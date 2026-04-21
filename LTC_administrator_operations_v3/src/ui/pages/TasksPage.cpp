#include "TasksPage.h"
#include "../../data/DatabaseManager.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
struct TaskSource {
    QString table;
    QString department;
    QString duty;
    QString dueCol;
    QString ownerCol;
    QString itemCol;
    QString statusCol;
};
}

TasksPage::TasksPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Tasks / Cross-Module Worklist", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel("This page pulls operational work from the other tabs into one running task board. You can still add and delete direct task items here.", this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* board = new QTableWidget(this);
    const QStringList boardCols{"Department", "Duty", "Due / Date", "Owner", "Item", "Status", "Source"};
    board->setColumnCount(boardCols.size());
    board->setHorizontalHeaderLabels(boardCols);
    board->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    root->addWidget(board, 1);

    auto* manualLabel = new QLabel("Direct tasks", this);
    manualLabel->setStyleSheet("font-size: 16px; font-weight: 700;");
    root->addWidget(manualLabel);

    auto* manualTable = new QTableWidget(this);
    const QStringList manualCols{"id", "due_date", "owner", "task_name", "priority", "status"};
    manualTable->setColumnCount(manualCols.size());
    manualTable->setHorizontalHeaderLabels({"ID", "Due", "Owner", "Task", "Priority", "Status"});
    manualTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    root->addWidget(manualTable);

    auto* form = new QHBoxLayout();
    auto* dueEdit = new QLineEdit(this);
    dueEdit->setPlaceholderText("YYYY-MM-DD");
    auto* ownerEdit = new QLineEdit(this);
    ownerEdit->setPlaceholderText("Owner");
    auto* taskEdit = new QLineEdit(this);
    taskEdit->setPlaceholderText("Task");
    auto* priorityEdit = new QLineEdit(this);
    priorityEdit->setPlaceholderText("Priority");
    auto* addButton = new QPushButton("Add Task", this);
    auto* deleteButton = new QPushButton("Delete Selected Task", this);
    form->addWidget(dueEdit);
    form->addWidget(ownerEdit);
    form->addWidget(taskEdit);
    form->addWidget(priorityEdit);
    form->addWidget(addButton);
    form->addWidget(deleteButton);
    root->addLayout(form);

    auto refreshBoards = [=]() {
        board->setRowCount(0);
        const QList<TaskSource> sources = {
            {"tasks", "General", "Follow-up", "due_date", "owner", "task_name", "status"},
            {"social_services_items", "Social Services", "Discharge / family / psychosocial", "review_date", "owner", "item_name", "status"},
            {"grievances", "Social Services", "Grievance follow-up", "report_date", "owner", "summary", "status"},
            {"care_conference_items", "Social Services", "Care conference", "conference_date", "owner_name", "conference_type", "status"},
            {"environmental_rounds", "Environmental Services", "Maintenance / rounds", "round_date", "owner", "issue_name", "status"},
            {"housekeeping_laundry_items", "Environmental Services", "Laundry / housekeeping", "review_date", "owner", "item_name", "status"},
            {"pharmacy_items", "Medical Records", "Pharmacy / med orders / DNR", "review_date", "owner", "item_name", "status"},
            {"vaccination_items", "Medical Records", "Vaccination follow-up", "review_date", "resident_name", "vaccine_name", "status"},
            {"isolation_items", "Medical Records", "Isolation follow-up", "review_date", "resident_name", "isolation_type", "status"},
            {"infection_control_items", "Medical Records", "Infection-control record", "id", "owner", "item_name", "status"},
            {"document_items", "Medical Records", "Document intake", "document_date", "owner", "document_name", "status"},
            {"outbreak_items", "Medical Records", "Outbreak review", "event_date", "unit_name", "event_name", "status"},
            {"mds_items", "MDS", "Assessment / triple check", "ard_date", "owner", "assessment_type", "status"},
            {"therapy_items", "MDS", "Therapy / rehab", "review_date", "owner", "item_name", "status"},
            {"transportation_items", "Transportation", "Appointment / transport", "appt_date", "owner", "item_name", "status"},
            {"incidents", "DON", "Incident follow-up", "incident_date", "resident_name", "incident_type", "status"},
            {"interventions", "DON", "Intervention", "review_date", "owner", "intervention_name", "status"},
            {"diagnosis_reportables", "DON", "Diagnosis reportable", "review_date", "owner", "diagnosis_name", "status"},
            {"wound_treatments", "Treatments", "Wound treatment", "review_date", "resident_name", "wound_name", "status"},
            {"compliance_items", "Compliance", "Compliance follow-up", "due_date", "owner", "item_name", "status"},
            {"quality_followups", "Quality", "Quality follow-up", "followup_date", "owner", "item_name", "status"}
        };
        for (const auto& src : sources) {
            const QStringList cols{src.dueCol, src.ownerCol, src.itemCol, src.statusCol};
            const auto rows = db->fetchTable(src.table, cols);
            for (const auto& row : rows) {
                const int r = board->rowCount();
                board->insertRow(r);
                board->setItem(r, 0, new QTableWidgetItem(src.department));
                board->setItem(r, 1, new QTableWidgetItem(src.duty));
                board->setItem(r, 2, new QTableWidgetItem(row.value(src.dueCol)));
                board->setItem(r, 3, new QTableWidgetItem(row.value(src.ownerCol)));
                board->setItem(r, 4, new QTableWidgetItem(row.value(src.itemCol)));
                board->setItem(r, 5, new QTableWidgetItem(row.value(src.statusCol)));
                board->setItem(r, 6, new QTableWidgetItem(src.table));
            }
        }

        manualTable->setRowCount(0);
        const auto manualRows = db->fetchTable("tasks", manualCols);
        for (const auto& row : manualRows) {
            const int r = manualTable->rowCount();
            manualTable->insertRow(r);
            for (int c = 0; c < manualCols.size(); ++c) manualTable->setItem(r, c, new QTableWidgetItem(row.value(manualCols[c])));
        }
    };

    QObject::connect(addButton, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{{"due_date", dueEdit->text().trimmed()}, {"owner", ownerEdit->text().trimmed()}, {"task_name", taskEdit->text().trimmed()}, {"priority", priorityEdit->text().trimmed().isEmpty() ? "Medium" : priorityEdit->text().trimmed()}, {"status", "Open"}};
        if (db->addRecord("tasks", values)) {
            dueEdit->clear(); ownerEdit->clear(); taskEdit->clear(); priorityEdit->clear();
            refreshBoards();
        }
    });

    QObject::connect(deleteButton, &QPushButton::clicked, this, [=]() {
        const int row = manualTable->currentRow();
        if (row < 0) return;
        bool ok = false;
        const int id = manualTable->item(row, 0)->text().toInt(&ok);
        if (ok && db->deleteRecordById("tasks", id)) refreshBoards();
    });

    refreshBoards();
}
