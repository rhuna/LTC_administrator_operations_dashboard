#include "SurveyLiveResponseTrackerPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QColor>
#include <QComboBox>
#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QTime>
#include <QVBoxLayout>

SurveyLiveResponseTrackerPage::SurveyLiveResponseTrackerPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Survey Live Response Tracker", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v76 adds a live survey response tracker so leadership can log surveyor requests in real time, assign immediate ownership, watch due-soon and overdue items, and mark requests delivered without losing the survey-day trail.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet(
        "background:#eef4f8; border:1px solid #d9e2ec; border-radius:10px;"
        "padding:8px 14px; color:#334e68; font-weight:600;");

    root->addWidget(heading);
    root->addWidget(subheading);
    root->addWidget(m_summaryLabel);

    auto* strip = new QHBoxLayout();
    auto makeStat = [&](const QString& title, QLabel*& valueLabel, const QString& hintText) {
        auto* card = new QGroupBox(title, this);
        auto* layout = new QVBoxLayout(card);
        valueLabel = new QLabel("0", card);
        valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #12344d;");
        auto* hint = new QLabel(hintText, card);
        hint->setWordWrap(true);
        hint->setStyleSheet("color:#5b6472;");
        layout->addWidget(valueLabel);
        layout->addWidget(hint);
        strip->addWidget(card);
    };
    makeStat("Open requests", m_openLabel, "Requests still open, assigned, gathering, or due soon during live survey activity.");
    makeStat("Due soon", m_dueSoonLabel, "Open live requests that need delivery today or are explicitly marked due soon.");
    makeStat("Overdue", m_overdueLabel, "Open live requests that should already have been delivered.");
    makeStat("Delivered today", m_completedLabel, "Requests marked delivered or closed during the current survey day.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Log live survey request", this);
    auto* formLayout = new QFormLayout(formCard);
    m_requestDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_requestTimeEdit = new QLineEdit(QTime::currentTime().toString("HH:mm"), formCard);
    m_requestSourceEdit = new QLineEdit(formCard);
    m_categoryEdit = new QLineEdit(formCard);
    m_descriptionEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_dueTimeEdit = new QLineEdit(QTime::currentTime().addSecs(30 * 60).toString("HH:mm"), formCard);
    m_deliveryNoteEdit = new QTextEdit(formCard);
    m_deliveryNoteEdit->setPlaceholderText("What was requested, where it was pulled from, hand-delivery notes, or escalation detail.");
    m_deliveryNoteEdit->setFixedHeight(84);

    m_priorityCombo = new QComboBox(formCard);
    m_priorityCombo->addItems({"Urgent", "High", "Medium", "Low"});

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Assigned", "Gathering", "Due Soon", "Delivered", "Closed"});

    formLayout->addRow("Request date", m_requestDateEdit);
    formLayout->addRow("Request time", m_requestTimeEdit);
    formLayout->addRow("Surveyor / source", m_requestSourceEdit);
    formLayout->addRow("Category", m_categoryEdit);
    formLayout->addRow("Request description", m_descriptionEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Due time", m_dueTimeEdit);
    formLayout->addRow("Priority", m_priorityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Delivery note", m_deliveryNoteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save live request", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Live survey request board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Request Date", "Time", "Source", "Category", "Description", "Owner", "Due Time", "Status"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_deliveredButton = new QPushButton("Mark Delivered", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_deliveredButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &SurveyLiveResponseTrackerPage::addRequest);
    QObject::connect(m_deliveredButton, &QPushButton::clicked, this, &SurveyLiveResponseTrackerPage::markSelectedDelivered);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &SurveyLiveResponseTrackerPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &SurveyLiveResponseTrackerPage::refreshBoard);

    refreshBoard();
}

void SurveyLiveResponseTrackerPage::addRequest() {
    if (!m_db) return;
    const QString source = m_requestSourceEdit->text().trimmed();
    const QString category = m_categoryEdit->text().trimmed();
    const QString description = m_descriptionEdit->text().trimmed();
    if (source.isEmpty() || category.isEmpty() || description.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Source, category, and request description are required.");
        return;
    }

    if (!m_db->addRecord("survey_live_requests", {
            {"request_date", m_requestDateEdit->text().trimmed()},
            {"request_time", m_requestTimeEdit->text().trimmed()},
            {"request_source", source},
            {"request_category", category},
            {"request_description", description},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"due_time", m_dueTimeEdit->text().trimmed()},
            {"priority", m_priorityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"delivery_note", m_deliveryNoteEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The live survey request could not be saved.");
        return;
    }

    m_requestSourceEdit->clear();
    m_categoryEdit->clear();
    m_descriptionEdit->clear();
    m_ownerEdit->clear();
    m_dueTimeEdit->setText(QTime::currentTime().addSecs(30 * 60).toString("HH:mm"));
    m_priorityCombo->setCurrentText("Urgent");
    m_statusCombo->setCurrentText("Open");
    m_deliveryNoteEdit->clear();
    refreshBoard();
}

void SurveyLiveResponseTrackerPage::markSelectedDelivered() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a live request first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("survey_live_requests", id, {{"status", "Delivered"}});
    refreshBoard();
}

void SurveyLiveResponseTrackerPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a live request first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this live survey request?") == QMessageBox::Yes) {
        m_db->deleteRecordById("survey_live_requests", id);
        refreshBoard();
    }
}

void SurveyLiveResponseTrackerPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "survey_live_requests",
        {"id", "request_date", "request_time", "request_source", "request_category", "request_description", "owner_name", "due_time", "delivery_note", "priority", "status"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const QString nowTime = QTime::currentTime().toString("HH:mm");
    const QString openFilter = "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'";
    const int openCount = m_db->countWhere("survey_live_requests", openFilter);
    const int dueSoonCount = m_db->countWhere("survey_live_requests", QString("(%1) AND request_date='%2' AND due_time <= '%3'").arg(openFilter, today, nowTime));
    const int overdueCount = m_db->countWhere("survey_live_requests", QString("(%1) AND request_date < '%2'").arg(openFilter, today))
        + m_db->countWhere("survey_live_requests", QString("(%1) AND request_date='%2' AND due_time < '%3'").arg(openFilter, today, nowTime));
    const int deliveredCount = m_db->countWhere("survey_live_requests", "status='Delivered' OR status='Closed'");

    m_openLabel->setText(QString::number(openCount));
    m_dueSoonLabel->setText(QString::number(dueSoonCount));
    m_overdueLabel->setText(QString::number(overdueCount));
    m_completedLabel->setText(QString::number(deliveredCount));

    m_summaryLabel->setText(
        QString("%1 open live request(s)  ·  %2 due soon  ·  %3 overdue  ·  %4 delivered/closed  ·  %5 command-center item(s) remain active across the broader survey workflow")
            .arg(openCount)
            .arg(dueSoonCount)
            .arg(overdueCount)
            .arg(deliveredCount)
            .arg(m_db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("request_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* timeItem = new QTableWidgetItem(row.value("request_time"));
        auto* sourceItem = new QTableWidgetItem(row.value("request_source"));
        auto* categoryItem = new QTableWidgetItem(row.value("request_category"));
        auto* descriptionItem = new QTableWidgetItem(row.value("request_description"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* dueItem = new QTableWidgetItem(row.value("due_time"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString priority = row.value("priority");
        const QString status = row.value("status");
        if (priority == "Urgent") descriptionItem->setForeground(QColor("#9a3412"));
        else if (priority == "High") descriptionItem->setForeground(QColor("#92400e"));

        if (status == "Delivered" || status == "Closed") statusItem->setForeground(QColor("#166534"));
        else if (status == "Due Soon") statusItem->setForeground(QColor("#92400e"));
        else if (status == "Gathering") statusItem->setForeground(QColor("#0b4f8a"));

        const QString deliveryNote = row.value("delivery_note").trimmed();
        if (!deliveryNote.isEmpty()) descriptionItem->setToolTip(deliveryNote);
        const QString dueTime = row.value("due_time");
        if (!dueTime.isEmpty()) dueItem->setToolTip(QString("Priority: %1").arg(priority));

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, timeItem);
        m_table->setItem(r, 2, sourceItem);
        m_table->setItem(r, 3, categoryItem);
        m_table->setItem(r, 4, descriptionItem);
        m_table->setItem(r, 5, ownerItem);
        m_table->setItem(r, 6, dueItem);
        m_table->setItem(r, 7, statusItem);
    }
}
