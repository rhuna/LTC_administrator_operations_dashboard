#include "SurveyDocumentRequestLogPage.h"
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

SurveyDocumentRequestLogPage::SurveyDocumentRequestLogPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Survey Document Request Log", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v77 adds a dedicated document request log so leadership can track the exact policies, rosters, binders, logs, and evidence packets surveyors asked for, who is pulling them, where they are located, and whether the material has been delivered.",
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
    makeStat("Open document pulls", m_openLabel, "Requests still open, locating, copying, assembling, or queued for delivery.");
    makeStat("Due today", m_dueTodayLabel, "Document pulls leadership should finish today for surveyor hand-off.");
    makeStat("Missing / blocked", m_missingLabel, "Requests marked missing, blocked, or not immediately available.");
    makeStat("Delivered", m_deliveredLabel, "Document requests already delivered or closed out.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Log document request", this);
    auto* formLayout = new QFormLayout(formCard);
    m_requestDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_requestTimeEdit = new QLineEdit(QTime::currentTime().toString("HH:mm"), formCard);
    m_requestSourceEdit = new QLineEdit(formCard);
    m_documentTypeEdit = new QLineEdit(formCard);
    m_documentNameEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_locationEdit = new QLineEdit(formCard);
    m_dueDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_noteEdit = new QTextEdit(formCard);
    m_noteEdit->setPlaceholderText("Where the document is stored, whether it needs printing/redaction, or what is still blocking delivery.");
    m_noteEdit->setFixedHeight(84);

    m_priorityCombo = new QComboBox(formCard);
    m_priorityCombo->addItems({"Urgent", "High", "Medium", "Low"});

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Locating", "Printing", "Ready to Deliver", "Missing", "Delivered", "Closed"});

    formLayout->addRow("Request date", m_requestDateEdit);
    formLayout->addRow("Request time", m_requestTimeEdit);
    formLayout->addRow("Surveyor / source", m_requestSourceEdit);
    formLayout->addRow("Document type", m_documentTypeEdit);
    formLayout->addRow("Document / packet", m_documentNameEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Location / source", m_locationEdit);
    formLayout->addRow("Due date", m_dueDateEdit);
    formLayout->addRow("Priority", m_priorityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Notes", m_noteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save document request", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Document request board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Request Date", "Time", "Source", "Type", "Document", "Owner", "Due Date", "Status"
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

    QObject::connect(m_addButton, &QPushButton::clicked, this, &SurveyDocumentRequestLogPage::addRequest);
    QObject::connect(m_deliveredButton, &QPushButton::clicked, this, &SurveyDocumentRequestLogPage::markSelectedDelivered);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &SurveyDocumentRequestLogPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &SurveyDocumentRequestLogPage::refreshBoard);

    refreshBoard();
}

void SurveyDocumentRequestLogPage::addRequest() {
    if (!m_db) return;
    const QString source = m_requestSourceEdit->text().trimmed();
    const QString docType = m_documentTypeEdit->text().trimmed();
    const QString docName = m_documentNameEdit->text().trimmed();
    if (source.isEmpty() || docType.isEmpty() || docName.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Source, document type, and document name are required.");
        return;
    }

    if (!m_db->addRecord("survey_document_requests", {
            {"request_date", m_requestDateEdit->text().trimmed()},
            {"request_time", m_requestTimeEdit->text().trimmed()},
            {"request_source", source},
            {"document_type", docType},
            {"document_name", docName},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"source_location", m_locationEdit->text().trimmed()},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"priority", m_priorityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"notes", m_noteEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The survey document request could not be saved.");
        return;
    }

    m_requestSourceEdit->clear();
    m_documentTypeEdit->clear();
    m_documentNameEdit->clear();
    m_ownerEdit->clear();
    m_locationEdit->clear();
    m_dueDateEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_priorityCombo->setCurrentText("Urgent");
    m_statusCombo->setCurrentText("Open");
    m_noteEdit->clear();
    refreshBoard();
}

void SurveyDocumentRequestLogPage::markSelectedDelivered() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a document request first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("survey_document_requests", id, {{"status", "Delivered"}});
    refreshBoard();
}

void SurveyDocumentRequestLogPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a document request first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this survey document request?") == QMessageBox::Yes) {
        m_db->deleteRecordById("survey_document_requests", id);
        refreshBoard();
    }
}

void SurveyDocumentRequestLogPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "survey_document_requests",
        {"id", "request_date", "request_time", "request_source", "document_type", "document_name", "owner_name", "source_location", "due_date", "priority", "status", "notes"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const QString openFilter = "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'";
    const int openCount = m_db->countWhere("survey_document_requests", openFilter);
    const int dueTodayCount = m_db->countWhere("survey_document_requests", QString("(%1) AND due_date <= '%2'").arg(openFilter, today));
    const int missingCount = m_db->countWhere("survey_document_requests", "status='Missing'");
    const int deliveredCount = m_db->countWhere("survey_document_requests", "status='Delivered' OR status='Closed'");

    m_openLabel->setText(QString::number(openCount));
    m_dueTodayLabel->setText(QString::number(dueTodayCount));
    m_missingLabel->setText(QString::number(missingCount));
    m_deliveredLabel->setText(QString::number(deliveredCount));

    m_summaryLabel->setText(
        QString("%1 open document request(s)  ·  %2 due today  ·  %3 missing / blocked  ·  %4 delivered  ·  %5 live survey request(s) remain active alongside document pulls")
            .arg(openCount)
            .arg(dueTodayCount)
            .arg(missingCount)
            .arg(deliveredCount)
            .arg(m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("request_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* timeItem = new QTableWidgetItem(row.value("request_time"));
        auto* sourceItem = new QTableWidgetItem(row.value("request_source"));
        auto* typeItem = new QTableWidgetItem(row.value("document_type"));
        auto* documentItem = new QTableWidgetItem(row.value("document_name"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* dueItem = new QTableWidgetItem(row.value("due_date"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString priority = row.value("priority");
        const QString status = row.value("status");
        if (priority == "Urgent") documentItem->setForeground(QColor("#9a3412"));
        else if (priority == "High") documentItem->setForeground(QColor("#92400e"));

        if (status == "Delivered" || status == "Closed") statusItem->setForeground(QColor("#166534"));
        else if (status == "Missing") statusItem->setForeground(QColor("#b91c1c"));
        else if (status == "Ready to Deliver") statusItem->setForeground(QColor("#0b4f8a"));

        const QString notes = row.value("notes").trimmed();
        const QString location = row.value("source_location").trimmed();
        QString tip = location;
        if (!notes.isEmpty()) {
            if (!tip.isEmpty()) tip += "\n";
            tip += notes;
        }
        if (!tip.isEmpty()) documentItem->setToolTip(tip);

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, timeItem);
        m_table->setItem(r, 2, sourceItem);
        m_table->setItem(r, 3, typeItem);
        m_table->setItem(r, 4, documentItem);
        m_table->setItem(r, 5, ownerItem);
        m_table->setItem(r, 6, dueItem);
        m_table->setItem(r, 7, statusItem);
    }
}
