#include "AlertsEscalationCenterPage.h"
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

AlertsEscalationCenterPage::AlertsEscalationCenterPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Alerts & Escalation Center", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v81 adds an urgency layer across the survey workflow so leadership can see what is overdue, blocked, critical, or still unresolved without opening every individual board first. Use it as the fast triage screen before huddles, entrance conference prep, or live survey response.",
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
    makeStat("Live alerts", m_totalLabel, "Tracked alert or escalation items still open across the center.");
    makeStat("Critical", m_criticalLabel, "Alerts marked critical or urgent enough to escalate immediately.");
    makeStat("Overdue / due now", m_overdueLabel, "Open items leadership should clear today or that have already slipped.");
    makeStat("Blocked", m_blockedLabel, "Items that cannot close without another department, document, or decision.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Log alert / escalation", this);
    auto* formLayout = new QFormLayout(formCard);
    m_alertDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_boardEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_dueDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_itemEdit = new QLineEdit(formCard);
    m_severityCombo = new QComboBox(formCard);
    m_severityCombo->addItems({"Medium", "High", "Critical"});
    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Due Today", "Blocked", "Resolved"});
    m_noteEdit = new QTextEdit(formCard);
    m_noteEdit->setFixedHeight(84);
    m_noteEdit->setPlaceholderText("What is stuck, who needs to respond, and what would clear the item today?");

    formLayout->addRow("Alert date", m_alertDateEdit);
    formLayout->addRow("Source board", m_boardEdit);
    formLayout->addRow("Item / alert", m_itemEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Due date", m_dueDateEdit);
    formLayout->addRow("Severity", m_severityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Escalation note", m_noteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save alert", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Escalation board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Alert Date", "Board", "Item", "Owner", "Due Date", "Severity", "Status"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_resolveButton = new QPushButton("Mark Resolved", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Center", this);
    actionBar->addWidget(m_resolveButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &AlertsEscalationCenterPage::addEscalation);
    QObject::connect(m_resolveButton, &QPushButton::clicked, this, &AlertsEscalationCenterPage::markSelectedResolved);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &AlertsEscalationCenterPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &AlertsEscalationCenterPage::refreshBoard);

    refreshBoard();
}

void AlertsEscalationCenterPage::addEscalation() {
    if (!m_db) return;
    const QString boardName = m_boardEdit->text().trimmed();
    const QString itemName = m_itemEdit->text().trimmed();
    if (boardName.isEmpty() || itemName.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Source board and item / alert are required.");
        return;
    }

    if (!m_db->addRecord("alerts_escalation_items", {
            {"alert_date", m_alertDateEdit->text().trimmed()},
            {"board_name", boardName},
            {"item_name", itemName},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"severity", m_severityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"escalation_note", m_noteEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The alert / escalation item could not be saved.");
        return;
    }

    m_boardEdit->clear();
    m_itemEdit->clear();
    m_ownerEdit->clear();
    m_dueDateEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_severityCombo->setCurrentText("Medium");
    m_statusCombo->setCurrentText("Open");
    m_noteEdit->clear();
    refreshBoard();
}

void AlertsEscalationCenterPage::markSelectedResolved() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select an alert first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("alerts_escalation_items", id, {{"status", "Resolved"}});
    refreshBoard();
}

void AlertsEscalationCenterPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select an alert first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this alert / escalation entry?") == QMessageBox::Yes) {
        m_db->deleteRecordById("alerts_escalation_items", id);
        refreshBoard();
    }
}

void AlertsEscalationCenterPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "alerts_escalation_items",
        {"id", "alert_date", "board_name", "item_name", "owner_name", "due_date", "severity", "status", "escalation_note"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const QString openFilter = "status='Open' OR status='Due Today' OR status='Blocked'";
    const int openCount = m_db->countWhere("alerts_escalation_items", openFilter);
    const int criticalCount = m_db->countWhere("alerts_escalation_items", QString("(%1) AND severity='Critical'").arg(openFilter));
    const int overdueCount = m_db->countWhere("alerts_escalation_items", QString("(%1) AND due_date <= '%2'").arg(openFilter, today));
    const int blockedCount = m_db->countWhere("alerts_escalation_items", "status='Blocked'");

    const int liveRequestPressure = m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'");
    const int docMissing = m_db->countWhere("survey_document_requests", "status='Missing'");
    const int tracerHigh = m_db->countWhere("resident_tracer_items", "(status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated') AND (risk_level='High' OR risk_level='Critical')");
    const int pocOverdue = m_db->countWhere("plan_of_correction_items", QString("(status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review') AND due_date < '%1'").arg(today));

    m_totalLabel->setText(QString::number(openCount));
    m_criticalLabel->setText(QString::number(criticalCount));
    m_overdueLabel->setText(QString::number(overdueCount));
    m_blockedLabel->setText(QString::number(blockedCount));

    m_summaryLabel->setText(
        QString("%1 live alert(s)  ·  %2 critical  ·  %3 due now / overdue  ·  %4 blocked  ·  %5 active live-survey request(s)  ·  %6 missing document pull(s)  ·  %7 high-risk tracer(s)  ·  %8 overdue POC item(s)")
            .arg(openCount)
            .arg(criticalCount)
            .arg(overdueCount)
            .arg(blockedCount)
            .arg(liveRequestPressure)
            .arg(docMissing)
            .arg(tracerHigh)
            .arg(pocOverdue));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("alert_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* boardItem = new QTableWidgetItem(row.value("board_name"));
        auto* itemItem = new QTableWidgetItem(row.value("item_name"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* dueItem = new QTableWidgetItem(row.value("due_date"));
        auto* severityItem = new QTableWidgetItem(row.value("severity"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString severity = row.value("severity");
        const QString status = row.value("status");
        if (severity == "Critical") severityItem->setForeground(QColor("#b42318"));
        else if (severity == "High") severityItem->setForeground(QColor("#b54708"));
        if (status == "Blocked") statusItem->setForeground(QColor("#92400e"));
        else if (status == "Resolved") statusItem->setForeground(QColor("#166534"));
        else if (status == "Due Today") statusItem->setForeground(QColor("#0b4f8a"));

        const QString note = row.value("escalation_note").trimmed();
        if (!note.isEmpty()) {
            itemItem->setToolTip(note);
            boardItem->setToolTip(note);
        }

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, boardItem);
        m_table->setItem(r, 2, itemItem);
        m_table->setItem(r, 3, ownerItem);
        m_table->setItem(r, 4, dueItem);
        m_table->setItem(r, 5, severityItem);
        m_table->setItem(r, 6, statusItem);
    }
}
