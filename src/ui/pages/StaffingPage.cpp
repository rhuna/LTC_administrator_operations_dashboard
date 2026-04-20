#include "StaffingPage.h"
#include "../../data/DatabaseManager.h"

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
#include <QVBoxLayout>

StaffingPage::StaffingPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Staffing Operations", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Add staffing assignments, flip open shifts to filled coverage, and monitor minimum staffing gaps in one place.", this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    m_snapshotLabel = new QLabel(this);
    m_snapshotLabel->setStyleSheet("font-weight: 600; color: #334155;");
    root->addWidget(heading);
    root->addWidget(subheading);
    root->addWidget(m_snapshotLabel);

    auto* formCard = new QGroupBox("Add staffing assignment", this);
    auto* formLayout = new QFormLayout(formCard);
    m_dateEdit = new QLineEdit(formCard);
    m_departmentEdit = new QLineEdit(formCard);
    m_shiftEdit = new QLineEdit(formCard);
    m_roleEdit = new QLineEdit(formCard);
    m_employeeEdit = new QLineEdit(formCard);
    m_statusEdit = new QLineEdit(formCard);

    m_dateEdit->setPlaceholderText("YYYY-MM-DD");
    m_departmentEdit->setPlaceholderText("Nursing / Dietary / EVS");
    m_shiftEdit->setPlaceholderText("Day / Evening / Night");
    m_roleEdit->setPlaceholderText("CNA / RN/LPN / Cook");
    m_employeeEdit->setPlaceholderText("Employee or Open Position");
    m_statusEdit->setPlaceholderText("Scheduled / Filled / Open");
    m_statusEdit->setText("Scheduled");

    formLayout->addRow("Work date:", m_dateEdit);
    formLayout->addRow("Department:", m_departmentEdit);
    formLayout->addRow("Shift:", m_shiftEdit);
    formLayout->addRow("Role:", m_roleEdit);
    formLayout->addRow("Employee:", m_employeeEdit);
    formLayout->addRow("Status:", m_statusEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Add Assignment", formCard);
    m_markOpenButton = new QPushButton("Mark Selected Open", formCard);
    m_markFilledButton = new QPushButton("Mark Selected Filled", formCard);
    m_refreshButton = new QPushButton("Refresh", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_markOpenButton);
    buttonRow->addWidget(m_markFilledButton);
    buttonRow->addWidget(m_refreshButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* assignmentsCard = new QGroupBox("Assignments and coverage status", this);
    auto* assignmentsLayout = new QVBoxLayout(assignmentsCard);
    m_assignmentsTable = new QTableWidget(assignmentsCard);
    m_assignmentsTable->setColumnCount(7);
    m_assignmentsTable->setHorizontalHeaderLabels(QStringList{"id", "work_date", "department", "shift_name", "role_name", "employee_name", "status"});
    m_assignmentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_assignmentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_assignmentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_assignmentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_assignmentsTable->setColumnHidden(0, true);
    assignmentsLayout->addWidget(m_assignmentsTable);
    root->addWidget(assignmentsCard, 1);

    auto* minimumCard = new QGroupBox("Minimum staffing summary", this);
    auto* minimumLayout = new QVBoxLayout(minimumCard);
    auto* minimumHint = new QLabel("This summary highlights assignment groups that are currently below the configured minimum staffing level.", minimumCard);
    minimumHint->setWordWrap(true);
    minimumHint->setStyleSheet("color: #5b6472;");
    m_minimumsTable = new QTableWidget(minimumCard);
    m_minimumsTable->setColumnCount(6);
    m_minimumsTable->setHorizontalHeaderLabels(QStringList{"department", "shift_name", "role_name", "minimum_required", "scheduled_count", "gap_count"});
    m_minimumsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_minimumsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    minimumLayout->addWidget(minimumHint);
    minimumLayout->addWidget(m_minimumsTable);
    root->addWidget(minimumCard, 1);

    connect(m_addButton, &QPushButton::clicked, this, &StaffingPage::handleAddAssignment);
    connect(m_markOpenButton, &QPushButton::clicked, this, &StaffingPage::handleMarkSelectedOpen);
    connect(m_markFilledButton, &QPushButton::clicked, this, &StaffingPage::handleMarkSelectedFilled);
    connect(m_refreshButton, &QPushButton::clicked, this, &StaffingPage::refreshTables);

    refreshTables();
}

void StaffingPage::refreshTables() {
    m_assignmentsTable->setRowCount(0);
    const auto rows = m_db->fetchTable("staffing_assignments", QStringList{"id", "work_date", "department", "shift_name", "role_name", "employee_name", "status"});
    for (const auto& row : rows) {
        int r = m_assignmentsTable->rowCount();
        m_assignmentsTable->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"id", "work_date", "department", "shift_name", "role_name", "employee_name", "status"}) {
            m_assignmentsTable->setItem(r, c++, new QTableWidgetItem(row.value(key)));
        }
    }

    m_minimumsTable->setRowCount(0);
    const auto minimumRows = m_db->staffingMinimumSummary();
    for (const auto& row : minimumRows) {
        int r = m_minimumsTable->rowCount();
        m_minimumsTable->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"department", "shift_name", "role_name", "minimum_required", "scheduled_count", "gap_count"}) {
            m_minimumsTable->setItem(r, c++, new QTableWidgetItem(row.value(key)));
        }
    }

    m_snapshotLabel->setText(
        QString("%1 open staffing assignment(s) · %2 minimum staffing gap group(s)")
            .arg(m_db->countWhere("staffing_assignments", "status='Open'"))
            .arg(m_db->countMinimumStaffingGaps()));
}

void StaffingPage::handleAddAssignment() {
    if (m_departmentEdit->text().trimmed().isEmpty() || m_shiftEdit->text().trimmed().isEmpty() || m_roleEdit->text().trimmed().isEmpty()) {
        QMessageBox::information(this, "Add staffing assignment", "Enter at least department, shift, and role.");
        return;
    }

    const QString employee = m_employeeEdit->text().trimmed().isEmpty() ? QString("Open Position") : m_employeeEdit->text().trimmed();
    const QString status = m_statusEdit->text().trimmed().isEmpty() ? QString("Scheduled") : m_statusEdit->text().trimmed();

    if (!m_db->addStaffingAssignment(m_dateEdit->text().trimmed(), m_departmentEdit->text().trimmed(), m_shiftEdit->text().trimmed(),
                                     m_roleEdit->text().trimmed(), employee, status)) {
        QMessageBox::warning(this, "Add staffing assignment", "Unable to save the assignment.");
        return;
    }

    refreshTables();
}

void StaffingPage::handleMarkSelectedOpen() {
    const int row = m_assignmentsTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Update staffing", "Select an assignment row first.");
        return;
    }
    const int id = m_assignmentsTable->item(row, 0)->text().toInt();
    if (!m_db->updateStaffingAssignmentStatus(id, "Open")) {
        QMessageBox::warning(this, "Update staffing", "Unable to mark the selected assignment open.");
        return;
    }
    refreshTables();
}

void StaffingPage::handleMarkSelectedFilled() {
    const int row = m_assignmentsTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Update staffing", "Select an assignment row first.");
        return;
    }
    const int id = m_assignmentsTable->item(row, 0)->text().toInt();
    if (!m_db->updateStaffingAssignmentStatus(id, "Filled")) {
        QMessageBox::warning(this, "Update staffing", "Unable to mark the selected assignment filled.");
        return;
    }
    refreshTables();
}
