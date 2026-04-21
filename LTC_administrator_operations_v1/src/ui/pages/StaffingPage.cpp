#include "StaffingPage.h"
#include "../../data/DatabaseManager.h"

#include <QColor>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {
QString norm(const QString& value, const QString& fallback) {
    const QString trimmed = value.trimmed();
    return trimmed.isEmpty() ? fallback : trimmed;
}

QLabel* makeStatValue(const QString& value, QWidget* parent) {
    auto* label = new QLabel(value, parent);
    label->setStyleSheet("font-size: 26px; font-weight: 700; color: #0f172a;");
    return label;
}

QLabel* makeStatCaption(const QString& text, QWidget* parent) {
    auto* label = new QLabel(text, parent);
    label->setWordWrap(true);
    label->setStyleSheet("font-size: 11px; color: #64748b;");
    return label;
}
}

StaffingPage::StaffingPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Staffing Operations", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Manage assignments, flip coverage status, and review minimum staffing, estimated hours, and nursing HPRD-style staffing numbers in one workspace.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    m_snapshotLabel = new QLabel(this);
    m_snapshotLabel->setStyleSheet("font-weight: 600; color: #334155;");
    root->addWidget(heading);
    root->addWidget(subheading);
    root->addWidget(m_snapshotLabel);

    auto* statRow = new QHBoxLayout();
    statRow->setSpacing(10);
    auto addStatCard = [&](const QString& caption, QLabel** outValue) {
        auto* card = new QFrame(this);
        card->setStyleSheet("QFrame { background: #f8fafc; border: 1px solid #e2e8f0; border-radius: 14px; }");
        auto* layout = new QVBoxLayout(card);
        layout->setContentsMargins(14, 12, 14, 12);
        layout->setSpacing(4);
        *outValue = makeStatValue("0", card);
        layout->addWidget(*outValue);
        layout->addWidget(makeStatCaption(caption, card));
        statRow->addWidget(card, 1);
    };
    addStatCard("Open assignments needing coverage", &m_openCountLabel);
    addStatCard("Shift groups below minimum staffing", &m_gapCountLabel);
    addStatCard("Assignments currently marked filled", &m_filledCountLabel);
    addStatCard("Agency-covered assignments", &m_agencyCountLabel);
    addStatCard("Estimated nursing HPRD", &m_hprdLabel);
    root->addLayout(statRow);

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
    m_roleEdit->setPlaceholderText("CNA / RN / LPN / Cook");
    m_employeeEdit->setPlaceholderText("Employee name, Agency Pool, or Open Position");
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

    auto* rowTwo = new QHBoxLayout();
    rowTwo->setSpacing(14);

    auto* shiftTotalsCard = new QGroupBox("Shift-by-shift staffing totals", this);
    auto* shiftTotalsLayout = new QVBoxLayout(shiftTotalsCard);
    auto* shiftHint = new QLabel("Totals are grouped by department, shift, and role so you can see what is covered, still open, and estimated in hours.", shiftTotalsCard);
    shiftHint->setWordWrap(true);
    shiftHint->setStyleSheet("color: #5b6472;");
    m_shiftTotalsTable = new QTableWidget(shiftTotalsCard);
    m_shiftTotalsTable->setColumnCount(7);
    m_shiftTotalsTable->setHorizontalHeaderLabels(QStringList{"department", "shift_name", "role_name", "total", "filled", "open", "scheduled"});
    m_shiftTotalsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_shiftTotalsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    shiftTotalsLayout->addWidget(shiftHint);
    shiftTotalsLayout->addWidget(m_shiftTotalsTable);
    rowTwo->addWidget(shiftTotalsCard, 3);

    auto* mixCard = new QGroupBox("Coverage mix", this);
    auto* mixLayout = new QVBoxLayout(mixCard);
    auto* mixHint = new QLabel("Quick split of employee, agency, and open coverage by department.", mixCard);
    mixHint->setWordWrap(true);
    mixHint->setStyleSheet("color: #5b6472;");
    m_mixTable = new QTableWidget(mixCard);
    m_mixTable->setColumnCount(4);
    m_mixTable->setHorizontalHeaderLabels(QStringList{"department", "employee", "agency", "open"});
    m_mixTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_mixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mixLayout->addWidget(mixHint);
    mixLayout->addWidget(m_mixTable);
    rowTwo->addWidget(mixCard, 2);

    root->addLayout(rowTwo, 1);

    auto* rowThree = new QHBoxLayout();
    rowThree->setSpacing(14);

    auto* minimumCard = new QGroupBox("Minimum staffing summary", this);
    auto* minimumLayout = new QVBoxLayout(minimumCard);
    auto* minimumHint = new QLabel("This compares currently covered assignments to configured minimum staffing targets and shows the remaining gap.", minimumCard);
    minimumHint->setWordWrap(true);
    minimumHint->setStyleSheet("color: #5b6472;");
    m_minimumsTable = new QTableWidget(minimumCard);
    m_minimumsTable->setColumnCount(6);
    m_minimumsTable->setHorizontalHeaderLabels(QStringList{"department", "shift_name", "role_name", "minimum_required", "scheduled_count", "gap_count"});
    m_minimumsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_minimumsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    minimumLayout->addWidget(minimumHint);
    minimumLayout->addWidget(m_minimumsTable);
    rowThree->addWidget(minimumCard, 3);

    auto* hprdCard = new QGroupBox("Nursing HPRD view", this);
    auto* hprdLayout = new QVBoxLayout(hprdCard);
    auto* hprdHint = new QLabel("Estimated nursing hours per resident day based on currently covered nursing assignments at 8 hours each.", hprdCard);
    hprdHint->setWordWrap(true);
    hprdHint->setStyleSheet("color: #5b6472;");
    m_hprdTable = new QTableWidget(hprdCard);
    m_hprdTable->setColumnCount(4);
    m_hprdTable->setHorizontalHeaderLabels(QStringList{"role_name", "covered_count", "estimated_hours", "hprd"});
    m_hprdTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_hprdTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    hprdLayout->addWidget(hprdHint);
    hprdLayout->addWidget(m_hprdTable);
    rowThree->addWidget(hprdCard, 2);

    root->addLayout(rowThree, 1);

    auto* hoursCard = new QGroupBox("Estimated staffing hours by shift", this);
    auto* hoursLayout = new QVBoxLayout(hoursCard);
    auto* hoursHint = new QLabel("Estimated hours assume an 8-hour assignment. Use this to see where covered hours and open hours are concentrated.", hoursCard);
    hoursHint->setWordWrap(true);
    hoursHint->setStyleSheet("color: #5b6472;");
    m_hoursTable = new QTableWidget(hoursCard);
    m_hoursTable->setColumnCount(6);
    m_hoursTable->setHorizontalHeaderLabels(QStringList{"department", "shift_name", "role_name", "covered_count", "estimated_hours", "open_hours"});
    m_hoursTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_hoursTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    hoursLayout->addWidget(hoursHint);
    hoursLayout->addWidget(m_hoursTable);
    root->addWidget(hoursCard, 1);

    connect(m_addButton, &QPushButton::clicked, this, &StaffingPage::handleAddAssignment);
    connect(m_markOpenButton, &QPushButton::clicked, this, &StaffingPage::handleMarkSelectedOpen);
    connect(m_markFilledButton, &QPushButton::clicked, this, &StaffingPage::handleMarkSelectedFilled);
    connect(m_refreshButton, &QPushButton::clicked, this, &StaffingPage::refreshTables);

    refreshTables();
}

void StaffingPage::refreshTables() {
    const QStringList assignmentCols{"id", "work_date", "department", "shift_name", "role_name", "employee_name", "status"};
    const auto rows = m_db->fetchTable("staffing_assignments", assignmentCols);

    m_assignmentsTable->setRowCount(0);
    QMap<QString, QMap<QString, int>> groupedCounts;
    QMap<QString, QMap<QString, int>> departmentMix;
    int openCount = 0;
    int filledCount = 0;
    int agencyCount = 0;

    for (const auto& row : rows) {
        const int r = m_assignmentsTable->rowCount();
        m_assignmentsTable->insertRow(r);
        for (int c = 0; c < assignmentCols.size(); ++c) {
            m_assignmentsTable->setItem(r, c, new QTableWidgetItem(row.value(assignmentCols[c])));
        }

        const QString department = norm(row.value("department"), "Unknown");
        const QString shift = norm(row.value("shift_name"), "Unknown");
        const QString role = norm(row.value("role_name"), "Unknown");
        const QString employee = norm(row.value("employee_name"), "Open Position");
        const QString status = norm(row.value("status"), "Scheduled");
        const QString groupKey = department + "|" + shift + "|" + role;

        groupedCounts[groupKey]["total"] += 1;
        if (status.compare("Filled", Qt::CaseInsensitive) == 0) {
            groupedCounts[groupKey]["filled"] += 1;
            filledCount += 1;
        }
        if (status.compare("Open", Qt::CaseInsensitive) == 0) {
            groupedCounts[groupKey]["open"] += 1;
            openCount += 1;
        }
        if (status.compare("Scheduled", Qt::CaseInsensitive) == 0) {
            groupedCounts[groupKey]["scheduled"] += 1;
        }

        const bool isAgency = employee.contains("agency", Qt::CaseInsensitive) || employee.contains("pool", Qt::CaseInsensitive);
        const bool isOpen = status.compare("Open", Qt::CaseInsensitive) == 0 || employee.contains("open", Qt::CaseInsensitive);
        if (isAgency) {
            departmentMix[department]["agency"] += 1;
            agencyCount += 1;
        } else if (isOpen) {
            departmentMix[department]["open"] += 1;
        } else {
            departmentMix[department]["employee"] += 1;
        }
    }

    m_shiftTotalsTable->setRowCount(0);
    const auto groupKeys = groupedCounts.keys();
    for (const QString& key : groupKeys) {
        const QStringList parts = key.split('|');
        if (parts.size() != 3) continue;
        const int r = m_shiftTotalsTable->rowCount();
        m_shiftTotalsTable->insertRow(r);
        m_shiftTotalsTable->setItem(r, 0, new QTableWidgetItem(parts[0]));
        m_shiftTotalsTable->setItem(r, 1, new QTableWidgetItem(parts[1]));
        m_shiftTotalsTable->setItem(r, 2, new QTableWidgetItem(parts[2]));
        m_shiftTotalsTable->setItem(r, 3, new QTableWidgetItem(QString::number(groupedCounts[key].value("total"))));
        m_shiftTotalsTable->setItem(r, 4, new QTableWidgetItem(QString::number(groupedCounts[key].value("filled"))));
        m_shiftTotalsTable->setItem(r, 5, new QTableWidgetItem(QString::number(groupedCounts[key].value("open"))));
        m_shiftTotalsTable->setItem(r, 6, new QTableWidgetItem(QString::number(groupedCounts[key].value("scheduled"))));
    }

    m_mixTable->setRowCount(0);
    const auto departments = departmentMix.keys();
    for (const QString& department : departments) {
        const int r = m_mixTable->rowCount();
        m_mixTable->insertRow(r);
        m_mixTable->setItem(r, 0, new QTableWidgetItem(department));
        m_mixTable->setItem(r, 1, new QTableWidgetItem(QString::number(departmentMix[department].value("employee"))));
        m_mixTable->setItem(r, 2, new QTableWidgetItem(QString::number(departmentMix[department].value("agency"))));
        m_mixTable->setItem(r, 3, new QTableWidgetItem(QString::number(departmentMix[department].value("open"))));
    }

    m_minimumsTable->setRowCount(0);
    const auto minimumRows = m_db->staffingMinimumSummary();
    for (const auto& row : minimumRows) {
        const int r = m_minimumsTable->rowCount();
        m_minimumsTable->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"department", "shift_name", "role_name", "minimum_required", "scheduled_count", "gap_count"}) {
            auto* item = new QTableWidgetItem(row.value(key));
            if (key == "gap_count" && row.value(key).toInt() > 0) {
                item->setBackground(QColor("#fee2e2"));
            }
            m_minimumsTable->setItem(r, c++, item);
        }
    }

    m_hprdTable->setRowCount(0);
    const auto hprdRows = m_db->nursingHprdSummary();
    for (const auto& row : hprdRows) {
        const int r = m_hprdTable->rowCount();
        m_hprdTable->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"role_name", "covered_count", "estimated_hours", "hprd"}) {
            m_hprdTable->setItem(r, c++, new QTableWidgetItem(row.value(key)));
        }
    }

    m_hoursTable->setRowCount(0);
    const auto hoursRows = m_db->staffingHoursSummary();
    for (const auto& row : hoursRows) {
        const int r = m_hoursTable->rowCount();
        m_hoursTable->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"department", "shift_name", "role_name", "covered_count", "estimated_hours", "open_hours"}) {
            auto* item = new QTableWidgetItem(row.value(key));
            if (key == "open_hours" && row.value(key).toInt() > 0) {
                item->setBackground(QColor("#fff7ed"));
            }
            m_hoursTable->setItem(r, c++, item);
        }
    }

    const int minimumGaps = m_db->countMinimumStaffingGaps();
    const int minimumGapHours = m_db->estimatedMinimumHoursGap();
    const double nursingHprd = m_db->estimatedNursingHprd();
    m_snapshotLabel->setText(
        QString("%1 open assignment(s) · %2 minimum staffing gap group(s) · %3 uncovered minimum hours · Nursing HPRD %4")
            .arg(openCount)
            .arg(minimumGaps)
            .arg(minimumGapHours)
            .arg(QString::number(nursingHprd, 'f', 2)));
    m_openCountLabel->setText(QString::number(openCount));
    m_gapCountLabel->setText(QString::number(minimumGaps));
    m_filledCountLabel->setText(QString::number(filledCount));
    m_agencyCountLabel->setText(QString::number(agencyCount));
    m_hprdLabel->setText(QString::number(nursingHprd, 'f', 2));
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

    m_dateEdit->clear();
    m_departmentEdit->clear();
    m_shiftEdit->clear();
    m_roleEdit->clear();
    m_employeeEdit->clear();
    m_statusEdit->setText("Scheduled");
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
