#include "HuddlePage.h"
#include "../../data/DatabaseManager.h"

#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
struct HuddleSource {
    QString table;
    QString department;
    QString duty;
    QString dueCol;
    QString ownerCol;
    QString itemCol;
    QString statusCol;
};
}

HuddlePage::HuddlePage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Daily Ops Huddle / Department Task Board", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel("Use this page as the daily huddle board. It combines direct huddle items, cross-module tasks, department-view rollups, and metrics/trend snapshots so leadership can scan daily priorities in one place.", this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* summary = new QLabel(this);
    summary->setWordWrap(true);
    summary->setStyleSheet("background:#eef4f8; border:1px solid #d9e2ec; border-radius:10px; padding:8px 12px; color:#334e68; font-weight:600;");
    const int residents = db->fetchTable("residents", {"status"}).size();
    const int openTasks = db->fetchTable("tasks", {"status"}).size();
    const int offTargetQuality = db->fetchTable("quality_measures", {"status"}).size();
    const int surveyRisk = db->fetchTable("survey_items", {"status"}).size();
    summary->setText(QString("Department view summary  ·  Residents tracked: %1  ·  Task rows: %2  ·  Quality measures: %3  ·  Survey-ready rows: %4").arg(residents).arg(openTasks).arg(offTargetQuality).arg(surveyRisk));
    root->addWidget(summary);

    auto* huddleTable = new QTableWidget(this);
    const QStringList cols{"Department", "Duty", "Date", "Owner", "Topic / Task", "Priority / Status", "Source"};
    huddleTable->setColumnCount(cols.size());
    huddleTable->setHorizontalHeaderLabels(cols);
    huddleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    root->addWidget(huddleTable);

    const QList<HuddleSource> sources = {
        {"huddle_items", "Operations", "Existing huddle issue", "huddle_date", "owner", "topic", "status"},
        {"tasks", "General", "Task follow-up", "due_date", "owner", "task_name", "status"},
        {"social_services_items", "Social Services", "Discharge / psychosocial", "review_date", "owner", "item_name", "status"},
        {"grievances", "Social Services", "Grievance", "report_date", "owner", "summary", "status"},
        {"care_conference_items", "Social Services", "Care conference", "conference_date", "owner_name", "conference_type", "status"},
        {"environmental_rounds", "Environmental Services", "Maintenance / rounds", "round_date", "owner", "issue_name", "status"},
        {"housekeeping_laundry_items", "Environmental Services", "Laundry / housekeeping", "review_date", "owner", "item_name", "status"},
        {"pharmacy_items", "Medical Records", "Pharmacy", "review_date", "owner", "item_name", "status"},
        {"infection_control_items", "Medical Records", "Infection control", "id", "owner", "item_name", "status"},
        {"mds_items", "MDS", "Assessment / triple check", "ard_date", "owner", "assessment_type", "status"},
        {"therapy_items", "MDS", "Therapy / rehab", "review_date", "owner", "item_name", "status"},
        {"incidents", "Nursing", "Incident", "event_date", "owner", "incident_type", "status"},
        {"transportation_items", "Transportation", "Appointment / transport", "appt_date", "owner", "item_name", "status"},
        {"dietary_items", "Dietary", "Nutrition task", "review_date", "owner", "item_name", "status"},
        {"kpi_trend_rows", "Leadership", "Metrics / trends", "period_label", "owner_name", "measure_name", "status"}
    };

    int rowIndex = 0;
    for (const auto& src : sources) {
        const QStringList sourceCols{src.dueCol, src.ownerCol, src.itemCol, src.statusCol};
        const auto rows = db->fetchTable(src.table, sourceCols);
        for (const auto& row : rows) {
            huddleTable->insertRow(rowIndex);
            huddleTable->setItem(rowIndex, 0, new QTableWidgetItem(src.department));
            huddleTable->setItem(rowIndex, 1, new QTableWidgetItem(src.duty));
            huddleTable->setItem(rowIndex, 2, new QTableWidgetItem(row.value(src.dueCol)));
            huddleTable->setItem(rowIndex, 3, new QTableWidgetItem(row.value(src.ownerCol)));
            huddleTable->setItem(rowIndex, 4, new QTableWidgetItem(row.value(src.itemCol)));
            huddleTable->setItem(rowIndex, 5, new QTableWidgetItem(row.value(src.statusCol)));
            huddleTable->setItem(rowIndex, 6, new QTableWidgetItem(src.table));
            ++rowIndex;
        }
    }
}
