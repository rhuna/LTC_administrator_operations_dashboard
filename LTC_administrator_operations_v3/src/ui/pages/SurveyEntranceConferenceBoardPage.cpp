#include "SurveyEntranceConferenceBoardPage.h"
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
#include <QVBoxLayout>

SurveyEntranceConferenceBoardPage::SurveyEntranceConferenceBoardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Survey Entrance Conference Board", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v74 adds a survey entrance conference board so leadership can stage documents, interview rosters, requested reports, and meeting-room readiness before surveyors ask for them.",
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
    makeStat("Open requests", m_openLabel, "Entrance-conference items still open, assigned, or actively being assembled for survey arrival.");
    makeStat("Due now", m_dueLabel, "Open requests that are due today or already overdue before the conference starts.");
    makeStat("Ready", m_readyLabel, "Packets, reports, rosters, and room-readiness items fully staged for conference use.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Log entrance-conference item", this);
    auto* formLayout = new QFormLayout(formCard);
    m_prepDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_requestEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_locationEdit = new QLineEdit(formCard);
    m_dueDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_noteEdit = new QTextEdit(formCard);
    m_noteEdit->setPlaceholderText("Requested report, packet contents, meeting-room setup, or surveyor cue note.");
    m_noteEdit->setFixedHeight(84);

    m_priorityCombo = new QComboBox(formCard);
    m_priorityCombo->addItems({"High", "Medium", "Low"});

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Assigned", "Assembling", "Ready"});

    formLayout->addRow("Prep date", m_prepDateEdit);
    formLayout->addRow("Request / item", m_requestEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Location / room", m_locationEdit);
    formLayout->addRow("Due date", m_dueDateEdit);
    formLayout->addRow("Priority", m_priorityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Notes", m_noteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save entrance item", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Entrance conference readiness board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Prep Date", "Request / Item", "Owner", "Location", "Due Date", "Priority", "Status"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_readyButton = new QPushButton("Mark Ready", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_readyButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &SurveyEntranceConferenceBoardPage::addConferenceItem);
    QObject::connect(m_readyButton, &QPushButton::clicked, this, &SurveyEntranceConferenceBoardPage::markSelectedReady);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &SurveyEntranceConferenceBoardPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &SurveyEntranceConferenceBoardPage::refreshBoard);

    refreshBoard();
}

void SurveyEntranceConferenceBoardPage::addConferenceItem() {
    if (!m_db) return;
    const QString request = m_requestEdit->text().trimmed();
    if (request.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Request / item is required.");
        return;
    }

    if (!m_db->addRecord("survey_entrance_conference_items", {
            {"prep_date", m_prepDateEdit->text().trimmed()},
            {"request_item", request},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"location_note", m_locationEdit->text().trimmed()},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"priority", m_priorityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"details_note", m_noteEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The entrance-conference item could not be saved.");
        return;
    }

    m_requestEdit->clear();
    m_ownerEdit->clear();
    m_locationEdit->clear();
    m_dueDateEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_priorityCombo->setCurrentText("High");
    m_statusCombo->setCurrentText("Open");
    m_noteEdit->clear();
    refreshBoard();
}

void SurveyEntranceConferenceBoardPage::markSelectedReady() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select an entrance-conference item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("survey_entrance_conference_items", id, {{"status", "Ready"}});
    refreshBoard();
}

void SurveyEntranceConferenceBoardPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select an entrance-conference item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this entrance-conference item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("survey_entrance_conference_items", id);
        refreshBoard();
    }
}

void SurveyEntranceConferenceBoardPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "survey_entrance_conference_items",
        {"id", "prep_date", "request_item", "owner_name", "location_note", "due_date", "details_note", "priority", "status"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const int openCount = m_db->countWhere("survey_entrance_conference_items", "status='Open' OR status='Assigned' OR status='Assembling'");
    const int dueCount = m_db->countWhere("survey_entrance_conference_items", QString("(status='Open' OR status='Assigned' OR status='Assembling') AND due_date <= '%1'").arg(today));
    const int readyCount = m_db->countWhere("survey_entrance_conference_items", "status='Ready'");

    m_openLabel->setText(QString::number(openCount));
    m_dueLabel->setText(QString::number(dueCount));
    m_readyLabel->setText(QString::number(readyCount));
    m_summaryLabel->setText(
        QString("Entrance conference summary: %1 open item(s), %2 due-now item(s), %3 ready item(s).")
            .arg(openCount).arg(dueCount).arg(readyCount));

    m_table->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows.at(r);
        auto* prepDateItem = new QTableWidgetItem(row.value("prep_date"));
        prepDateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* requestItem = new QTableWidgetItem(row.value("request_item"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* locationItem = new QTableWidgetItem(row.value("location_note"));
        auto* dueDateItem = new QTableWidgetItem(row.value("due_date"));
        auto* priorityItem = new QTableWidgetItem(row.value("priority"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString priority = row.value("priority");
        const QString status = row.value("status");
        if (priority == "High") priorityItem->setForeground(QColor("#92400e"));
        else if (priority == "Medium") priorityItem->setForeground(QColor("#0b4f8a"));
        else if (priority == "Low") priorityItem->setForeground(QColor("#166534"));

        if (status == "Ready") statusItem->setForeground(QColor("#166534"));
        else if (status == "Assembling") statusItem->setForeground(QColor("#0b4f8a"));
        else if (status == "Assigned") statusItem->setForeground(QColor("#7c3aed"));
        else if (status == "Open") statusItem->setForeground(QColor("#92400e"));

        const QString note = row.value("details_note").trimmed();
        if (!note.isEmpty()) requestItem->setToolTip(note);

        m_table->setItem(r, 0, prepDateItem);
        m_table->setItem(r, 1, requestItem);
        m_table->setItem(r, 2, ownerItem);
        m_table->setItem(r, 3, locationItem);
        m_table->setItem(r, 4, dueDateItem);
        m_table->setItem(r, 5, priorityItem);
        m_table->setItem(r, 6, statusItem);
    }
}
