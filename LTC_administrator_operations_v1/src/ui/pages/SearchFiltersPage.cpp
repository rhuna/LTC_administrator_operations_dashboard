#include "SearchFiltersPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
struct SearchModuleDef {
    QString label;
    QString table;
    QStringList columns;
    QString titleColumn;
    QString ownerColumn;
    QString statusColumn;
};

QList<SearchModuleDef> searchDefs() {
    return {
        {"Residents", "residents", {"resident_name", "room", "payer", "diagnosis_summary", "status"}, "resident_name", "room", "status"},
        {"Admissions", "admissions", {"resident_name", "referral_source", "planned_date", "payer", "diagnosis_summary", "status", "notes"}, "resident_name", "referral_source", "status"},
        {"Staffing", "staffing_assignments", {"work_date", "department", "shift_name", "role_name", "employee_name", "status"}, "role_name", "employee_name", "status"},
        {"Tasks", "tasks", {"due_date", "owner", "task_name", "priority", "status"}, "task_name", "owner", "status"},
        {"Incidents", "incidents", {"incident_date", "resident_name", "incident_type", "severity", "status"}, "incident_type", "resident_name", "status"},
        {"Quality", "quality_measures", {"measure_name", "category", "current_value", "target_value", "trend", "status", "notes"}, "measure_name", "category", "status"},
        {"Compliance", "compliance_items", {"item_name", "due_date", "owner", "status"}, "item_name", "owner", "status"},
        {"Documents", "document_items", {"module_name", "document_name", "document_type", "linked_item", "owner", "status", "notes"}, "document_name", "owner", "status"},
        {"MDS", "mds_items", {"resident_name", "payer", "assessment_type", "ard_date", "status", "owner", "notes"}, "resident_name", "owner", "status"},
        {"Survey Cmd", "survey_command_items", {"focus_area", "item_name", "evidence_needed", "owner", "priority", "due_date", "status", "notes"}, "item_name", "owner", "status"},
        {"Outbreak", "outbreak_items", {"issue_name", "location_name", "case_count", "owner", "priority", "review_date", "status", "notes"}, "issue_name", "owner", "status"}
    };
}
}

SearchFiltersPage::SearchFiltersPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Search & Filters", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Search across residents, referrals, staffing, quality, documents, MDS, survey command, and other high-use modules without leaving the dashboard shell.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* filterCard = new QGroupBox("Filter Results", this);
    auto* filterLayout = new QFormLayout(filterCard);

    m_moduleCombo = new QComboBox(filterCard);
    m_moduleCombo->addItem("All operational modules");
    for (const auto& def : searchDefs()) {
        m_moduleCombo->addItem(def.label);
    }

    m_statusCombo = new QComboBox(filterCard);
    m_statusCombo->addItems({"Any status", "Open", "In Progress", "Watch", "Due Soon", "Overdue", "Closed", "Ready", "Needs Docs", "Admitted", "Discharged", "At Risk", "Off Target"});

    m_keywordEdit = new QLineEdit(filterCard);
    m_keywordEdit->setPlaceholderText("Search resident names, items, diagnoses, documents, owners, notes, and more");

    auto* buttonRow = new QHBoxLayout();
    auto* applyButton = new QPushButton("Apply Filters", filterCard);
    auto* resetButton = new QPushButton("Reset", filterCard);
    buttonRow->addWidget(applyButton);
    buttonRow->addWidget(resetButton);
    buttonRow->addStretch(1);

    filterLayout->addRow("Module:", m_moduleCombo);
    filterLayout->addRow("Status:", m_statusCombo);
    filterLayout->addRow("Keyword:", m_keywordEdit);
    filterLayout->addRow(buttonRow);
    root->addWidget(filterCard);

    auto* summaryCard = new QFrame(this);
    summaryCard->setStyleSheet("QFrame { background:#f7fafc; border:1px solid #d9e2ec; border-radius:14px; }");
    auto* summaryLayout = new QVBoxLayout(summaryCard);
    summaryLayout->setContentsMargins(14, 12, 14, 12);
    m_summaryLabel = new QLabel(summaryCard);
    m_summaryLabel->setWordWrap(true);
    summaryLayout->addWidget(m_summaryLabel);
    root->addWidget(summaryCard);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"Module", "Primary Item", "Owner / Context", "Status", "Date / Detail", "Notes / Detail"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    root->addWidget(m_table, 1);

    connect(applyButton, &QPushButton::clicked, this, &SearchFiltersPage::applyFilters);
    connect(resetButton, &QPushButton::clicked, this, &SearchFiltersPage::resetFilters);
    connect(m_keywordEdit, &QLineEdit::returnPressed, this, &SearchFiltersPage::applyFilters);
    connect(m_moduleCombo, &QComboBox::currentIndexChanged, this, &SearchFiltersPage::applyFilters);
    connect(m_statusCombo, &QComboBox::currentIndexChanged, this, &SearchFiltersPage::applyFilters);

    populateTable();
}

void SearchFiltersPage::applyFilters() {
    populateTable();
}

void SearchFiltersPage::resetFilters() {
    m_moduleCombo->setCurrentIndex(0);
    m_statusCombo->setCurrentIndex(0);
    m_keywordEdit->clear();
    populateTable();
}

void SearchFiltersPage::populateTable() {
    const QString moduleFilter = m_moduleCombo->currentText();
    const QString statusFilter = m_statusCombo->currentText();
    const QString keyword = m_keywordEdit->text().trimmed().toLower();

    QList<QStringList> rows;
    int scannedModules = 0;

    for (const auto& def : searchDefs()) {
        if (moduleFilter != "All operational modules" && def.label != moduleFilter) {
            continue;
        }
        ++scannedModules;
        const auto sourceRows = m_db->fetchTable(def.table, def.columns);
        for (const auto& row : sourceRows) {
            const QString statusValue = row.value(def.statusColumn);
            if (statusFilter != "Any status" && statusValue.compare(statusFilter, Qt::CaseInsensitive) != 0) {
                continue;
            }

            bool keywordHit = keyword.isEmpty();
            if (!keywordHit) {
                for (const auto& col : def.columns) {
                    if (row.value(col).toLower().contains(keyword)) {
                        keywordHit = true;
                        break;
                    }
                }
            }
            if (!keywordHit) {
                continue;
            }

            QString detailValue;
            for (const auto& col : def.columns) {
                if (col != def.titleColumn && col != def.ownerColumn && col != def.statusColumn) {
                    const QString value = row.value(col).trimmed();
                    if (!value.isEmpty()) {
                        detailValue = value;
                        break;
                    }
                }
            }
            QString notesValue;
            if (row.contains("notes")) notesValue = row.value("notes");
            else if (row.contains("summary")) notesValue = row.value("summary");
            else if (row.contains("diagnosis_summary")) notesValue = row.value("diagnosis_summary");
            else if (row.contains("action_step")) notesValue = row.value("action_step");
            else if (row.contains("evidence_needed")) notesValue = row.value("evidence_needed");

            rows.append({def.label,
                         row.value(def.titleColumn),
                         row.value(def.ownerColumn),
                         statusValue,
                         detailValue,
                         notesValue});
        }
    }

    m_table->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < rows[r].size(); ++c) {
            m_table->setItem(r, c, new QTableWidgetItem(rows[r][c]));
        }
    }

    const QString summary = QString("Showing %1 result(s) across %2 module(s)%3%4.")
        .arg(rows.size())
        .arg(scannedModules)
        .arg(keyword.isEmpty() ? QString() : QString(" · keyword: \"%1\"").arg(m_keywordEdit->text().trimmed()))
        .arg(statusFilter == "Any status" ? QString() : QString(" · status: %1").arg(statusFilter));
    m_summaryLabel->setText(summary);
}
