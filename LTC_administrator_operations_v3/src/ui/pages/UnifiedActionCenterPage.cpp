#include "UnifiedActionCenterPage.h"
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

namespace {
QString safeValue(const QMap<QString, QString>& row, const QStringList& keys) {
    for (const QString& key : keys) {
        const QString value = row.value(key).trimmed();
        if (!value.isEmpty()) return value;
    }
    return QString();
}

QString joinNoteParts(const QStringList& parts) {
    QStringList filtered;
    for (const QString& part : parts) {
        if (!part.trimmed().isEmpty()) filtered << part.trimmed();
    }
    return filtered.join("\n");
}
}

UnifiedActionCenterPage::UnifiedActionCenterPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Unified Action Center", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v89 creates one executive action queue for the whole dashboard so leadership can see the most urgent work from daily operations, survey management, resident care, and support workflows in one place.",
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
    makeStat("Open pressure", m_openLabel, "Open high-importance items pulled together from alerts, barriers, live requests, tracers, POC work, and the manual action queue.");
    makeStat("Critical", m_criticalLabel, "Critical items across the connected workspaces that should be discussed first.");
    makeStat("Due today", m_dueTodayLabel, "Work that is due now or scheduled for today across the connected operational tabs.");
    makeStat("Blocked", m_blockedLabel, "Blocked or missing items that need a decision, escalation, or resource to move forward.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Add manual executive action", this);
    auto* formLayout = new QFormLayout(formCard);
    m_sourceEdit = new QLineEdit("Executive Follow-Up", formCard);
    m_itemEdit = new QLineEdit(formCard);
    m_itemEdit->setPlaceholderText("Short description of the item leadership needs to track.");
    m_ownerEdit = new QLineEdit("Administrator", formCard);
    m_dueDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_severityCombo = new QComboBox(formCard);
    m_severityCombo->addItems({"Critical", "High", "Medium", "Low"});
    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Due Today", "Blocked", "Resolved"});
    m_noteEdit = new QTextEdit(formCard);
    m_noteEdit->setPlaceholderText("Add notes, dependencies, or why the item matters.");
    m_noteEdit->setFixedHeight(84);

    formLayout->addRow("Source", m_sourceEdit);
    formLayout->addRow("Item", m_itemEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Due date", m_dueDateEdit);
    formLayout->addRow("Severity", m_severityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Notes", m_noteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save Manual Action", formCard);
    m_refreshButton = new QPushButton("Refresh Action Center", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_refreshButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* autoCard = new QGroupBox("Auto-generated priority feed", this);
    auto* autoLayout = new QVBoxLayout(autoCard);
    m_autoFeedTable = new QTableWidget(autoCard);
    m_autoFeedTable->setColumnCount(6);
    m_autoFeedTable->setHorizontalHeaderLabels({"Source", "Issue", "Owner", "Due", "Severity", "Status"});
    m_autoFeedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_autoFeedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_autoFeedTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_autoFeedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_autoFeedTable->setAlternatingRowColors(true);
    autoLayout->addWidget(m_autoFeedTable);
    root->addWidget(autoCard, 1);

    auto* manualCard = new QGroupBox("Manual executive action queue", this);
    auto* manualLayout = new QVBoxLayout(manualCard);
    m_manualQueueTable = new QTableWidget(manualCard);
    m_manualQueueTable->setColumnCount(6);
    m_manualQueueTable->setHorizontalHeaderLabels({"Source", "Item", "Owner", "Due Date", "Severity", "Status"});
    m_manualQueueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_manualQueueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_manualQueueTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_manualQueueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_manualQueueTable->setAlternatingRowColors(true);
    manualLayout->addWidget(m_manualQueueTable);
    root->addWidget(manualCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_resolveButton = new QPushButton("Mark Selected Resolved", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    actionBar->addWidget(m_resolveButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &UnifiedActionCenterPage::addManualAction);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &UnifiedActionCenterPage::refreshBoard);
    QObject::connect(m_resolveButton, &QPushButton::clicked, this, &UnifiedActionCenterPage::markSelectedResolved);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &UnifiedActionCenterPage::deleteSelectedManual);
    QObject::connect(m_db, &DatabaseManager::dataChanged, this, [this](const QString&) { refreshBoard(); });

    refreshBoard();
}

void UnifiedActionCenterPage::addManualAction() {
    if (!m_db) return;

    const QString source = m_sourceEdit->text().trimmed();
    const QString item = m_itemEdit->text().trimmed();
    const QString owner = m_ownerEdit->text().trimmed();
    if (source.isEmpty() || item.isEmpty() || owner.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Source, item, and owner are required.");
        return;
    }

    if (!m_db->addRecord("unified_action_items", {
            {"source_name", source},
            {"item_name", item},
            {"owner_name", owner},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"severity", m_severityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"note_text", m_noteEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The executive action item could not be saved.");
        return;
    }

    m_itemEdit->clear();
    m_noteEdit->clear();
    m_statusCombo->setCurrentText("Open");
    refreshBoard();
}

void UnifiedActionCenterPage::markSelectedResolved() {
    if (!m_db) return;
    const int row = m_manualQueueTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a manual action item first.");
        return;
    }
    const int id = m_manualQueueTable->item(row, 0) ? m_manualQueueTable->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("unified_action_items", id, {{"status", "Resolved"}});
    refreshBoard();
}

void UnifiedActionCenterPage::deleteSelectedManual() {
    if (!m_db) return;
    const int row = m_manualQueueTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a manual action item first.");
        return;
    }
    const int id = m_manualQueueTable->item(row, 0) ? m_manualQueueTable->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this manual executive action item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("unified_action_items", id);
        refreshBoard();
    }
}

void UnifiedActionCenterPage::populateAutoFeed() {
    if (!m_db || !m_autoFeedTable) return;
    m_autoFeedTable->setRowCount(0);

    auto addAutoRow = [&](const QString& source, const QString& issue, const QString& owner, const QString& due, const QString& severity, const QString& status, const QString& note) {
        const int row = m_autoFeedTable->rowCount();
        m_autoFeedTable->insertRow(row);

        auto* sourceItem = new QTableWidgetItem(source);
        auto* issueItem = new QTableWidgetItem(issue);
        auto* ownerItem = new QTableWidgetItem(owner);
        auto* dueItem = new QTableWidgetItem(due);
        auto* severityItem = new QTableWidgetItem(severity);
        auto* statusItem = new QTableWidgetItem(status);

        QColor tint = QColor("#fff4cc");
        if (severity == "Critical") tint = QColor("#fde8e8");
        else if (severity == "High") tint = QColor("#fff1d6");
        else if (status == "Blocked" || status == "Missing") tint = QColor("#ffe3e3");
        else if (status == "Due Today" || status == "Due Soon") tint = QColor("#eef7ff");

        for (auto* itemPtr : {sourceItem, issueItem, ownerItem, dueItem, severityItem, statusItem}) {
            itemPtr->setBackground(tint);
            itemPtr->setToolTip(note);
        }

        m_autoFeedTable->setItem(row, 0, sourceItem);
        m_autoFeedTable->setItem(row, 1, issueItem);
        m_autoFeedTable->setItem(row, 2, ownerItem);
        m_autoFeedTable->setItem(row, 3, dueItem);
        m_autoFeedTable->setItem(row, 4, severityItem);
        m_autoFeedTable->setItem(row, 5, statusItem);
    };

    const auto alerts = m_db->fetchTable("alerts_escalation_items", {"issue_name", "owner_name", "due_date", "severity", "status", "escalation_note"});
    for (const auto& row : alerts) {
        const QString status = row.value("status");
        if (status != "Resolved" && status != "Completed" && status != "Closed") {
            addAutoRow("Alerts & Escalation", row.value("issue_name"), row.value("owner_name"), row.value("due_date"),
                       row.value("severity").isEmpty() ? "High" : row.value("severity"), status, row.value("escalation_note"));
        }
    }

    const auto barriers = m_db->fetchTable("barrier_escalations", {"barrier_name", "owner_name", "entry_date", "severity", "status", "action_note", "target_outcome"});
    for (const auto& row : barriers) {
        const QString status = row.value("status");
        if (status != "Removed" && status != "Resolved" && status != "Closed") {
            addAutoRow("Barrier Escalation", row.value("barrier_name"), row.value("owner_name"), row.value("entry_date"),
                       row.value("severity").isEmpty() ? "High" : row.value("severity"), status,
                       joinNoteParts({row.value("action_note"), row.value("target_outcome")}));
        }
    }

    const auto live = m_db->fetchTable("survey_live_requests", {"request_description", "owner_name", "request_date", "due_time", "priority", "status", "delivery_note"});
    for (const auto& row : live) {
        const QString status = row.value("status");
        if (status != "Delivered" && status != "Completed" && status != "Closed") {
            addAutoRow("Survey Live Response", row.value("request_description"), row.value("owner_name"),
                       safeValue(row, {"due_time", "request_date"}),
                       row.value("priority").isEmpty() ? "High" : row.value("priority"),
                       status, row.value("delivery_note"));
        }
    }

    const auto docs = m_db->fetchTable("survey_document_requests", {"document_name", "owner_name", "due_date", "priority", "status", "notes", "source_location"});
    for (const auto& row : docs) {
        const QString status = row.value("status");
        if (status != "Delivered" && status != "Completed" && status != "Closed" && status != "Ready") {
            addAutoRow("Survey Document Requests", row.value("document_name"), row.value("owner_name"), row.value("due_date"),
                       row.value("priority").isEmpty() ? "Medium" : row.value("priority"),
                       status, joinNoteParts({row.value("source_location"), row.value("notes")}));
        }
    }

    const auto tracers = m_db->fetchTable("resident_tracer_items", {"resident_name", "focus_area", "owner_name", "tracer_date", "risk_level", "status", "tracer_note", "followup_action"});
    for (const auto& row : tracers) {
        const QString status = row.value("status");
        if (status != "Completed" && status != "Closed" && status != "Resolved") {
            addAutoRow("Resident Tracers",
                       QString("%1 - %2").arg(row.value("resident_name"), row.value("focus_area")),
                       row.value("owner_name"), row.value("tracer_date"),
                       row.value("risk_level").isEmpty() ? "Medium" : row.value("risk_level"),
                       status, joinNoteParts({row.value("tracer_note"), row.value("followup_action")}));
        }
    }

    const auto poc = m_db->fetchTable("plan_of_correction_items", {"citation_tag", "focus_area", "owner_name", "due_date", "severity", "status", "corrective_action", "evidence_plan"});
    for (const auto& row : poc) {
        const QString status = row.value("status");
        if (status != "Completed" && status != "Submitted" && status != "Closed") {
            addAutoRow("Plan of Correction",
                       QString("%1 - %2").arg(row.value("citation_tag"), row.value("focus_area")),
                       row.value("owner_name"), row.value("due_date"),
                       row.value("severity").isEmpty() ? "High" : row.value("severity"),
                       status, joinNoteParts({row.value("corrective_action"), row.value("evidence_plan")}));
        }
    }

    const auto followups = m_db->fetchTable("executive_followups", {"item_name", "owner_name", "due_date", "priority", "status", "source_name", "focus_area"});
    for (const auto& row : followups) {
        const QString status = row.value("status");
        if (status != "Complete" && status != "Closed" && status != "Completed") {
            addAutoRow("Executive Follow-Up",
                       row.value("item_name"), row.value("owner_name"), row.value("due_date"),
                       row.value("priority").isEmpty() ? "Medium" : row.value("priority"),
                       status, joinNoteParts({row.value("source_name"), row.value("focus_area")}));
        }
    }

    const auto morning = m_db->fetchTable("morning_meeting_items", {"item_name", "owner_name", "due_time", "priority", "status", "department_name", "board_date"});
    for (const auto& row : morning) {
        const QString status = row.value("status");
        if (status != "Complete" && status != "Closed" && status != "Completed") {
            addAutoRow("Morning Meeting",
                       row.value("item_name"), row.value("owner_name"),
                       safeValue(row, {"due_time", "board_date"}),
                       row.value("priority").isEmpty() ? "Medium" : row.value("priority"),
                       status, row.value("department_name"));
        }
    }

    m_autoFeedTable->sortItems(4, Qt::AscendingOrder);
}

void UnifiedActionCenterPage::populateManualQueue() {
    if (!m_db || !m_manualQueueTable) return;
    m_manualQueueTable->setRowCount(0);

    const auto rows = m_db->fetchTable(
        "unified_action_items",
        {"id", "source_name", "item_name", "owner_name", "due_date", "severity", "status", "note_text"});

    for (const auto& row : rows) {
        const int r = m_manualQueueTable->rowCount();
        m_manualQueueTable->insertRow(r);

        auto* sourceItem = new QTableWidgetItem(row.value("source_name"));
        sourceItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* itemItem = new QTableWidgetItem(row.value("item_name"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* dueItem = new QTableWidgetItem(row.value("due_date"));
        auto* severityItem = new QTableWidgetItem(row.value("severity"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString severity = row.value("severity");
        const QString status = row.value("status");
        QColor tint = QColor("#eef4f8");
        if (severity == "Critical") tint = QColor("#fde8e8");
        else if (severity == "High") tint = QColor("#fff1d6");
        if (status == "Resolved") tint = QColor("#dff3e4");
        else if (status == "Blocked") tint = QColor("#ffe3e3");

        for (auto* itemPtr : {sourceItem, itemItem, ownerItem, dueItem, severityItem, statusItem}) {
            itemPtr->setBackground(tint);
            itemPtr->setToolTip(row.value("note_text"));
        }

        m_manualQueueTable->setItem(r, 0, sourceItem);
        m_manualQueueTable->setItem(r, 1, itemItem);
        m_manualQueueTable->setItem(r, 2, ownerItem);
        m_manualQueueTable->setItem(r, 3, dueItem);
        m_manualQueueTable->setItem(r, 4, severityItem);
        m_manualQueueTable->setItem(r, 5, statusItem);
    }
}

void UnifiedActionCenterPage::refreshBoard() {
    if (!m_db) return;

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");

    const int alertsOpen = m_db->countWhere("alerts_escalation_items", "status='Open' OR status='Watching' OR status='Blocked' OR status='Due Today'");
    const int barriersOpen = m_db->countWhere("barrier_escalations", "status='Open' OR status='In Progress' OR status='Escalated' OR status='Waiting'");
    const int liveOpen = m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='In Progress' OR status='Due Soon'");
    const int docsOpen = m_db->countWhere("survey_document_requests", "status='Open' OR status='In Progress' OR status='Missing' OR status='Blocked'");
    const int tracersOpen = m_db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='At Risk' OR status='Needs Follow-Up'");
    const int pocOpen = m_db->countWhere("plan_of_correction_items", "status='Open' OR status='Drafting' OR status='Awaiting Evidence' OR status='Under Review'");
    const int manualOpen = m_db->countWhere("unified_action_items", "status='Open' OR status='Due Today' OR status='Blocked'");

    const int criticalCount =
        m_db->countWhere("alerts_escalation_items", "(status='Open' OR status='Watching' OR status='Blocked' OR status='Due Today') AND severity='Critical'") +
        m_db->countWhere("barrier_escalations", "(status='Open' OR status='In Progress' OR status='Escalated' OR status='Waiting') AND severity='Critical'") +
        m_db->countWhere("resident_tracer_items", "(status='Open' OR status='In Progress' OR status='At Risk' OR status='Needs Follow-Up') AND (risk_level='Critical' OR risk_level='High')") +
        m_db->countWhere("plan_of_correction_items", "(status='Open' OR status='Drafting' OR status='Awaiting Evidence' OR status='Under Review') AND severity='Critical'") +
        m_db->countWhere("unified_action_items", "(status='Open' OR status='Due Today' OR status='Blocked') AND severity='Critical'");

    const int dueTodayCount =
        m_db->countWhere("survey_document_requests", QString("(status='Open' OR status='In Progress' OR status='Missing' OR status='Blocked') AND due_date='%1'").arg(today)) +
        m_db->countWhere("executive_followups", QString("(status='Open' OR status='Assigned' OR status='Escalated') AND due_date='%1'").arg(today)) +
        m_db->countWhere("plan_of_correction_items", QString("(status='Open' OR status='Drafting' OR status='Awaiting Evidence' OR status='Under Review') AND due_date='%1'").arg(today)) +
        m_db->countWhere("unified_action_items", QString("(status='Open' OR status='Due Today' OR status='Blocked') AND due_date='%1'").arg(today));

    const int blockedCount =
        m_db->countWhere("alerts_escalation_items", "status='Blocked'") +
        m_db->countWhere("survey_document_requests", "status='Blocked' OR status='Missing'") +
        m_db->countWhere("morning_meeting_items", "status='Blocked'") +
        m_db->countWhere("unified_action_items", "status='Blocked'");

    const int openPressure = alertsOpen + barriersOpen + liveOpen + docsOpen + tracersOpen + pocOpen + manualOpen;

    m_openLabel->setText(QString::number(openPressure));
    m_criticalLabel->setText(QString::number(criticalCount));
    m_dueTodayLabel->setText(QString::number(dueTodayCount));
    m_blockedLabel->setText(QString::number(blockedCount));

    m_summaryLabel->setText(
        QString("%1 open pressure item(s) are being watched across the connected workspaces  ·  %2 critical  ·  %3 due today  ·  %4 blocked/missing  ·  %5 shared-record link(s) are currently active")
            .arg(openPressure)
            .arg(criticalCount)
            .arg(dueTodayCount)
            .arg(blockedCount)
            .arg(m_db->countWhere("shared_record_links")));

    populateAutoFeed();
    populateManualQueue();
}
