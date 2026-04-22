#include "BarrierEscalationBoardPage.h"
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

BarrierEscalationBoardPage::BarrierEscalationBoardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Barrier Escalation Board", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v70 adds a focused barrier-removal board so the administrator can separate true escalations from routine follow-up, assign ownership, and keep the day moving when blockers appear.",
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
    makeStat("Open barriers", m_openLabel, "Escalations still open, assigned, or waiting on action.");
    makeStat("Urgent", m_urgentLabel, "Barriers tagged high or critical severity.");
    makeStat("Removed", m_removedLabel, "Barriers closed out and removed today.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Log a barrier", this);
    auto* formLayout = new QFormLayout(formCard);
    m_dateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_departmentEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_barrierEdit = new QLineEdit(formCard);
    m_targetEdit = new QLineEdit(formCard);
    m_targetEdit->setPlaceholderText("What outcome or deadline should this unblock?");
    m_actionEdit = new QTextEdit(formCard);
    m_actionEdit->setPlaceholderText("Immediate next action, escalation path, or support needed.");
    m_actionEdit->setFixedHeight(84);

    m_severityCombo = new QComboBox(formCard);
    m_severityCombo->addItems({"Moderate", "High", "Critical"});
    m_severityCombo->setCurrentText("High");

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Assigned", "Waiting", "Removed"});

    formLayout->addRow("Date", m_dateEdit);
    formLayout->addRow("Department", m_departmentEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Barrier", m_barrierEdit);
    formLayout->addRow("Target outcome / due point", m_targetEdit);
    formLayout->addRow("Severity", m_severityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Immediate action", m_actionEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save barrier", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Escalation board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Date", "Department", "Owner", "Barrier", "Target", "Severity", "Status"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_removeButton = new QPushButton("Mark Removed", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_removeButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &BarrierEscalationBoardPage::addBarrier);
    QObject::connect(m_removeButton, &QPushButton::clicked, this, &BarrierEscalationBoardPage::markSelectedRemoved);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &BarrierEscalationBoardPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &BarrierEscalationBoardPage::refreshBoard);

    refreshBoard();
}

void BarrierEscalationBoardPage::addBarrier() {
    if (!m_db) return;
    const QString department = m_departmentEdit->text().trimmed();
    const QString barrier = m_barrierEdit->text().trimmed();
    if (department.isEmpty() || barrier.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Department and barrier are required.");
        return;
    }

    if (!m_db->addRecord("barrier_escalations", {
            {"entry_date", m_dateEdit->text().trimmed()},
            {"department_name", department},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"barrier_name", barrier},
            {"target_outcome", m_targetEdit->text().trimmed()},
            {"severity", m_severityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"action_note", m_actionEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The barrier escalation could not be saved.");
        return;
    }

    m_departmentEdit->clear();
    m_ownerEdit->clear();
    m_barrierEdit->clear();
    m_targetEdit->clear();
    m_actionEdit->clear();
    m_severityCombo->setCurrentText("High");
    m_statusCombo->setCurrentText("Open");
    refreshBoard();
}

void BarrierEscalationBoardPage::markSelectedRemoved() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a barrier first.");
        return;
    }

    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;

    m_db->updateRecordById("barrier_escalations", id, {{"status", "Removed"}});
    refreshBoard();
}

void BarrierEscalationBoardPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a barrier first.");
        return;
    }

    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;

    if (QMessageBox::question(this, "Confirm delete", "Delete this barrier escalation item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("barrier_escalations", id);
        refreshBoard();
    }
}

void BarrierEscalationBoardPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "barrier_escalations",
        {"id", "entry_date", "department_name", "owner_name", "barrier_name", "target_outcome", "action_note", "severity", "status"});

    const int openCount = m_db->countWhere("barrier_escalations", "status='Open' OR status='Assigned' OR status='Waiting'");
    const int urgentCount = m_db->countWhere("barrier_escalations", "severity='High' OR severity='Critical'");
    const int removedCount = m_db->countWhere("barrier_escalations", "status='Removed'");

    m_openLabel->setText(QString::number(openCount));
    m_urgentLabel->setText(QString::number(urgentCount));
    m_removedLabel->setText(QString::number(removedCount));

    m_summaryLabel->setText(
        QString("%1 open barrier(s)  ·  %2 urgent barrier(s)  ·  %3 removed barrier(s)  ·  %4 department pulse item(s) still need watch or escalation")
            .arg(openCount)
            .arg(urgentCount)
            .arg(removedCount)
            .arg(m_db->countWhere("department_pulse_items", "status='Open' OR status='Watching' OR status='Blocked'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("entry_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* departmentItem = new QTableWidgetItem(row.value("department_name"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* barrierItem = new QTableWidgetItem(row.value("barrier_name"));
        auto* targetItem = new QTableWidgetItem(row.value("target_outcome"));
        auto* severityItem = new QTableWidgetItem(row.value("severity"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString severity = row.value("severity");
        const QString status = row.value("status");
        if (severity == "Critical") severityItem->setForeground(QColor("#b91c1c"));
        else if (severity == "High") severityItem->setForeground(QColor("#92400e"));
        if (status == "Removed") statusItem->setForeground(QColor("#166534"));
        else if (status == "Waiting") statusItem->setForeground(QColor("#0b4f8a"));
        else if (status == "Open") statusItem->setForeground(QColor("#92400e"));

        const QString note = row.value("action_note").trimmed();
        if (!note.isEmpty()) barrierItem->setToolTip(note);

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, departmentItem);
        m_table->setItem(r, 2, ownerItem);
        m_table->setItem(r, 3, barrierItem);
        m_table->setItem(r, 4, targetItem);
        m_table->setItem(r, 5, severityItem);
        m_table->setItem(r, 6, statusItem);
    }
}
