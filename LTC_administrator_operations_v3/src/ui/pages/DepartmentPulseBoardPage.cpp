#include "DepartmentPulseBoardPage.h"
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

DepartmentPulseBoardPage::DepartmentPulseBoardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Department Pulse Board", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v69 adds a quick department pulse board so the administrator can see which departments are green, where risk is building, and what blockers need escalation before the day drifts.",
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
    makeStat("Active", m_activeLabel, "Departments with an open, watching, or blocked pulse item.");
    makeStat("Elevated risk", m_elevatedLabel, "Pulse items tagged high or critical risk.");
    makeStat("Blocked", m_blockedLabel, "Departments that need removal of a current blocker.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Update department pulse", this);
    auto* formLayout = new QFormLayout(formCard);
    m_dateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_departmentEdit = new QLineEdit(formCard);
    m_leaderEdit = new QLineEdit(formCard);
    m_stateEdit = new QLineEdit(formCard);
    m_blockerEdit = new QTextEdit(formCard);
    m_blockerEdit->setPlaceholderText("What is at risk, stalled, or needs escalation?");
    m_blockerEdit->setFixedHeight(84);

    m_riskCombo = new QComboBox(formCard);
    m_riskCombo->addItems({"Low", "Moderate", "High", "Critical"});
    m_riskCombo->setCurrentText("Moderate");

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Watching", "Blocked", "Resolved"});

    formLayout->addRow("Check-in date", m_dateEdit);
    formLayout->addRow("Department", m_departmentEdit);
    formLayout->addRow("Department leader", m_leaderEdit);
    formLayout->addRow("Current state", m_stateEdit);
    formLayout->addRow("Risk level", m_riskCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Blocker / escalation note", m_blockerEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save pulse item", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Department status board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Check-in", "Department", "Leader", "Current State", "Risk", "Status"
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
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_resolveButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &DepartmentPulseBoardPage::addPulseItem);
    QObject::connect(m_resolveButton, &QPushButton::clicked, this, &DepartmentPulseBoardPage::markSelectedResolved);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &DepartmentPulseBoardPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &DepartmentPulseBoardPage::refreshBoard);

    refreshBoard();
}

void DepartmentPulseBoardPage::addPulseItem() {
    if (!m_db) return;

    const QString department = m_departmentEdit->text().trimmed();
    const QString state = m_stateEdit->text().trimmed();
    if (department.isEmpty() || state.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Department and current state are required.");
        return;
    }

    if (!m_db->addRecord("department_pulse_items", {
            {"checkin_date", m_dateEdit->text().trimmed()},
            {"department_name", department},
            {"leader_name", m_leaderEdit->text().trimmed()},
            {"current_state", state},
            {"blocker_note", m_blockerEdit->toPlainText().trimmed()},
            {"risk_level", m_riskCombo->currentText()},
            {"status", m_statusCombo->currentText()}
        })) {
        QMessageBox::warning(this, "Save failed", "The department pulse item could not be saved.");
        return;
    }

    m_departmentEdit->clear();
    m_leaderEdit->clear();
    m_stateEdit->clear();
    m_blockerEdit->clear();
    m_riskCombo->setCurrentText("Moderate");
    m_statusCombo->setCurrentText("Open");
    refreshBoard();
}

void DepartmentPulseBoardPage::markSelectedResolved() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a pulse item first.");
        return;
    }

    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;

    m_db->updateRecordById("department_pulse_items", id, {{"status", "Resolved"}});
    refreshBoard();
}

void DepartmentPulseBoardPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a pulse item first.");
        return;
    }

    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;

    if (QMessageBox::question(this, "Confirm delete", "Delete this department pulse item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("department_pulse_items", id);
        refreshBoard();
    }
}

void DepartmentPulseBoardPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "department_pulse_items",
        {"id", "checkin_date", "department_name", "leader_name", "current_state", "blocker_note", "risk_level", "status"});

    const int activeCount = m_db->countWhere("department_pulse_items", "status='Open' OR status='Watching' OR status='Blocked'");
    const int elevatedCount = m_db->countWhere("department_pulse_items", "risk_level='High' OR risk_level='Critical'");
    const int blockedCount = m_db->countWhere("department_pulse_items", "status='Blocked'");

    m_activeLabel->setText(QString::number(activeCount));
    m_elevatedLabel->setText(QString::number(elevatedCount));
    m_blockedLabel->setText(QString::number(blockedCount));

    m_summaryLabel->setText(
        QString("%1 active department pulse item(s)  ·  %2 elevated-risk item(s)  ·  %3 blocked item(s)  ·  %4 morning-meeting item(s) still active for today")
            .arg(activeCount)
            .arg(elevatedCount)
            .arg(blockedCount)
            .arg(m_db->countWhere("morning_meeting_items", "status='Open' OR status='In Progress' OR status='Blocked'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("checkin_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* departmentItem = new QTableWidgetItem(row.value("department_name"));
        auto* leaderItem = new QTableWidgetItem(row.value("leader_name"));
        auto* stateItem = new QTableWidgetItem(row.value("current_state"));
        auto* riskItem = new QTableWidgetItem(row.value("risk_level"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString risk = row.value("risk_level");
        const QString status = row.value("status");
        if (risk == "Critical") riskItem->setForeground(QColor("#b91c1c"));
        else if (risk == "High") riskItem->setForeground(QColor("#92400e"));
        if (status == "Resolved") statusItem->setForeground(QColor("#166534"));
        else if (status == "Blocked") statusItem->setForeground(QColor("#b91c1c"));
        else if (status == "Watching") statusItem->setForeground(QColor("#0b4f8a"));

        const QString blocker = row.value("blocker_note").trimmed();
        if (!blocker.isEmpty()) stateItem->setToolTip(blocker);

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, departmentItem);
        m_table->setItem(r, 2, leaderItem);
        m_table->setItem(r, 3, stateItem);
        m_table->setItem(r, 4, riskItem);
        m_table->setItem(r, 5, statusItem);
    }
}
