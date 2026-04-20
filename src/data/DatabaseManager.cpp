#include "DatabaseManager.h"

#include <QDir>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QUuid>

namespace {
QString writableDataPath() {
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (basePath.isEmpty()) {
        basePath = QDir::currentPath() + "/data";
    }

    QDir dir(basePath);
    dir.mkpath(".");
    return dir.filePath("ltc_operations_dashboard.db");
}
}

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent), connectionName_(QUuid::createUuid().toString(QUuid::WithoutBraces)) {
}

DatabaseManager::~DatabaseManager() {
    if (database_.isValid() && database_.isOpen()) {
        database_.close();
    }
    if (!connectionName_.isEmpty()) {
        QSqlDatabase::removeDatabase(connectionName_);
    }
}

bool DatabaseManager::initialize() {
    return openDatabase() && createTables() && seedData();
}

QString DatabaseManager::databasePath() const {
    return writableDataPath();
}

QSqlDatabase DatabaseManager::database() const {
    return database_;
}

bool DatabaseManager::openDatabase() {
    database_ = QSqlDatabase::addDatabase("QSQLITE", connectionName_);
    database_.setDatabaseName(databasePath());
    return database_.open();
}

bool DatabaseManager::executeStatement(const QString& sql) const {
    QSqlQuery query(database_);
    return query.exec(sql);
}

bool DatabaseManager::createTables() {
    return executeStatement(
               "CREATE TABLE IF NOT EXISTS facility_info ("
               "id INTEGER PRIMARY KEY CHECK(id = 1),"
               "facility_name TEXT NOT NULL,"
               "bed_capacity INTEGER NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS residents ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name TEXT NOT NULL,"
               "payer_source TEXT NOT NULL,"
               "room TEXT NOT NULL,"
               "acuity TEXT NOT NULL,"
               "admit_date TEXT NOT NULL,"
               "status TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS staffing ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "department TEXT NOT NULL,"
               "scheduled INTEGER NOT NULL,"
               "worked INTEGER NOT NULL,"
               "agency INTEGER NOT NULL,"
               "overtime_hours REAL NOT NULL,"
               "hprd REAL NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS tasks ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "title TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "priority TEXT NOT NULL,"
               "due_date TEXT NOT NULL,"
               "status TEXT NOT NULL DEFAULT 'Open')") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS incidents ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "event_date TEXT NOT NULL,"
               "resident_name TEXT NOT NULL,"
               "severity TEXT NOT NULL,"
               "description TEXT NOT NULL,"
               "status TEXT NOT NULL DEFAULT 'Open')") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS survey_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "domain_name TEXT NOT NULL,"
               "item_name TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "readiness_status TEXT NOT NULL,"
               "next_review TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS admissions ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "resident_name TEXT NOT NULL,"
               "referral_source TEXT NOT NULL,"
               "payer_source TEXT NOT NULL,"
               "expected_date TEXT NOT NULL,"
               "status TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS qapi_projects ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "topic TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "source TEXT NOT NULL,"
               "stage TEXT NOT NULL,"
               "next_review TEXT NOT NULL,"
               "aim_statement TEXT NOT NULL,"
               "status TEXT NOT NULL DEFAULT 'Active')") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS budget_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "category TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "monthly_budget REAL NOT NULL,"
               "actual_spend REAL NOT NULL,"
               "status TEXT NOT NULL,"
               "next_review TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS compliance_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "area TEXT NOT NULL,"
               "item_name TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "due_date TEXT NOT NULL,"
               "status TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS staffing_changes ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "change_date TEXT NOT NULL,"
               "department TEXT NOT NULL,"
               "shift_name TEXT NOT NULL,"
               "change_type TEXT NOT NULL,"
               "position_name TEXT NOT NULL,"
               "employee_name TEXT NOT NULL,"
               "impact_level TEXT NOT NULL,"
               "status TEXT NOT NULL,"
               "notes TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS huddle_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "huddle_date TEXT NOT NULL,"
               "shift_name TEXT NOT NULL,"
               "department TEXT NOT NULL,"
               "topic TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "priority TEXT NOT NULL,"
               "status TEXT NOT NULL,"
               "notes TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS quality_measures ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "measure_name TEXT NOT NULL,"
               "measure_set TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "current_value REAL NOT NULL,"
               "target_value REAL NOT NULL,"
               "next_review TEXT NOT NULL,"
               "status TEXT NOT NULL,"
               "notes TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS training_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "training_area TEXT NOT NULL,"
               "employee_name TEXT NOT NULL,"
               "role_name TEXT NOT NULL,"
               "due_date TEXT NOT NULL,"
               "status TEXT NOT NULL,"
               "notes TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS emergency_preparedness_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "drill_date TEXT NOT NULL,"
               "scenario_name TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "next_drill_date TEXT NOT NULL,"
               "status TEXT NOT NULL,"
               "notes TEXT NOT NULL)") &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS infection_control_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "event_date TEXT NOT NULL,"
               "area_name TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "next_review TEXT NOT NULL,"
               "status TEXT NOT NULL,"
               "notes TEXT NOT NULL)");
}

bool DatabaseManager::seedData() {
    auto tableIsEmpty = [this](const QString& tableName) {
        QSqlQuery query(database_);
        query.exec(QString("SELECT COUNT(*) FROM %1").arg(tableName));
        return !query.next() || query.value(0).toInt() == 0;
    };

    auto executeAll = [this](const QStringList& statements) {
        for (const auto& sql : statements) {
            if (!executeStatement(sql)) {
                return false;
            }
        }
        return true;
    };

    if (tableIsEmpty("facility_info")) {
        if (!executeStatement("INSERT INTO facility_info (id, facility_name, bed_capacity) VALUES (1, 'Green Valley Care Center', 120)")) {
            return false;
        }
    }

    if (tableIsEmpty("residents")) {
        const QStringList residents = {
            "INSERT INTO residents (name, payer_source, room, acuity, admit_date, status) VALUES ('Mary Johnson', 'Medicare A', '101A', 'High', '2026-03-29', 'Current')",
            "INSERT INTO residents (name, payer_source, room, acuity, admit_date, status) VALUES ('Robert Smith', 'Medicaid', '101B', 'Moderate', '2025-11-18', 'Current')",
            "INSERT INTO residents (name, payer_source, room, acuity, admit_date, status) VALUES ('Alice Thomas', 'Managed Care', '102A', 'Low', '2026-04-03', 'Current')",
            "INSERT INTO residents (name, payer_source, room, acuity, admit_date, status) VALUES ('James Wilson', 'Medicaid', '103A', 'High', '2025-09-12', 'Current')",
            "INSERT INTO residents (name, payer_source, room, acuity, admit_date, status) VALUES ('Rosa Brown', 'Private Pay', '104B', 'Moderate', '2026-02-15', 'Current')",
            "INSERT INTO residents (name, payer_source, room, acuity, admit_date, status) VALUES ('David Garcia', 'Medicare A', '105A', 'High', '2026-04-11', 'Current')",
            "INSERT INTO residents (name, payer_source, room, acuity, admit_date, status) VALUES ('Lisa Miller', 'Managed Care', '106B', 'Low', '2026-01-23', 'Current')",
            "INSERT INTO residents (name, payer_source, room, acuity, admit_date, status) VALUES ('Evelyn Davis', 'Medicaid', '107A', 'Moderate', '2025-12-05', 'Current')"
        };
        if (!executeAll(residents)) return false;
    }

    if (tableIsEmpty("staffing")) {
        const QStringList staffing = {
            "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('Nursing', 28, 26, 3, 22.5, 3.85)",
            "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('CNA', 34, 31, 5, 27.0, 2.95)",
            "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('Dietary', 8, 8, 0, 2.0, 0.74)",
            "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('Housekeeping', 7, 6, 1, 4.5, 0.58)",
            "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('Activities', 3, 3, 0, 0.0, 0.21)"
        };
        if (!executeAll(staffing)) return false;
    }

    if (tableIsEmpty("tasks")) {
        const QStringList tasks = {
            "INSERT INTO tasks (title, owner, priority, due_date, status) VALUES ('Complete monthly QAPI packet', 'Administrator', 'High', '2026-04-24', 'Open')",
            "INSERT INTO tasks (title, owner, priority, due_date, status) VALUES ('Review infection control audit', 'DON', 'High', '2026-04-22', 'Open')",
            "INSERT INTO tasks (title, owner, priority, due_date, status) VALUES ('Renew dietary sanitation log', 'Dietary Manager', 'Medium', '2026-04-25', 'Open')",
            "INSERT INTO tasks (title, owner, priority, due_date, status) VALUES ('Finalize staffing contingency plan', 'Scheduler', 'Critical', '2026-04-21', 'Open')"
        };
        if (!executeAll(tasks)) return false;
    }

    if (tableIsEmpty("incidents")) {
        const QStringList incidents = {
            "INSERT INTO incidents (event_date, resident_name, severity, description, status) VALUES ('2026-04-18', 'Mary Johnson', 'Moderate', 'Unwitnessed fall with no apparent injury.', 'Open')",
            "INSERT INTO incidents (event_date, resident_name, severity, description, status) VALUES ('2026-04-16', 'James Wilson', 'Low', 'Medication variance identified during evening pass.', 'Closed')",
            "INSERT INTO incidents (event_date, resident_name, severity, description, status) VALUES ('2026-04-19', 'Rosa Brown', 'High', 'Elopement risk alarm malfunction found during round.', 'Open')"
        };
        if (!executeAll(incidents)) return false;
    }

    if (tableIsEmpty("survey_items")) {
        const QStringList survey = {
            "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Clinical', 'Accident / Supervision', 'DON', 'At Risk', '2026-04-21')",
            "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Clinical', 'Medication Storage', 'Unit Manager', 'Ready', '2026-04-28')",
            "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Life Safety', 'Fire Drill Documentation', 'Maintenance Director', 'At Risk', '2026-04-23')",
            "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Dietary', 'Temperature Logs', 'Dietary Manager', 'Ready', '2026-04-27')",
            "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Resident Rights', 'Grievance Tracking', 'Administrator', 'Needs Review', '2026-04-22')"
        };
        if (!executeAll(survey)) return false;
    }

    if (tableIsEmpty("admissions")) {
        const QStringList admissions = {
            "INSERT INTO admissions (resident_name, referral_source, payer_source, expected_date, status) VALUES ('Helen Carter', 'CHI St. Vincent', 'Medicare A', '2026-04-21', 'Pending')",
            "INSERT INTO admissions (resident_name, referral_source, payer_source, expected_date, status) VALUES ('Thomas Walker', 'Baptist Health', 'Managed Care', '2026-04-22', 'Pending')",
            "INSERT INTO admissions (resident_name, referral_source, payer_source, expected_date, status) VALUES ('Nina Howard', 'Community Referral', 'Private Pay', '2026-04-24', 'Pending')",
            "INSERT INTO admissions (resident_name, referral_source, payer_source, expected_date, status) VALUES ('George Lee', 'Hospital Readmit', 'Medicaid Pending', '2026-04-23', 'Planned Discharge')"
        };
        if (!executeAll(admissions)) return false;
    }

    if (tableIsEmpty("qapi_projects")) {
        const QStringList qapiProjects = {
            "INSERT INTO qapi_projects (topic, owner, source, stage, next_review, aim_statement, status) VALUES ('Reduce falls on hall 100', 'DON', 'Incident Review', 'Root Cause', '2026-04-23', 'Reduce unwitnessed falls by 25 percent over the next 60 days through rounding, risk review, and alarm checks.', 'Active')",
            "INSERT INTO qapi_projects (topic, owner, source, stage, next_review, aim_statement, status) VALUES ('Agency reduction plan', 'Administrator', 'Staffing', 'Intervention', '2026-04-25', 'Lower weekly agency utilization by stabilizing core schedule coverage and call-off escalation workflows.', 'Active')",
            "INSERT INTO qapi_projects (topic, owner, source, stage, next_review, aim_statement, status) VALUES ('Readmission prevention huddle', 'MDS Nurse', 'Readmission', 'Data Collection', '2026-04-24', 'Track top readmission diagnoses and strengthen 72-hour post-admit documentation reliability.', 'Active')"
        };
        if (!executeAll(qapiProjects)) return false;
    }

    if (tableIsEmpty("budget_items")) {
        const QStringList budgetItems = {
            "INSERT INTO budget_items (category, owner, monthly_budget, actual_spend, status, next_review) VALUES ('Nursing labor', 'Administrator', 148000.00, 154250.00, 'At Risk', '2026-04-22')",
            "INSERT INTO budget_items (category, owner, monthly_budget, actual_spend, status, next_review) VALUES ('Agency staffing', 'Scheduler', 18000.00, 24350.00, 'At Risk', '2026-04-21')",
            "INSERT INTO budget_items (category, owner, monthly_budget, actual_spend, status, next_review) VALUES ('Dietary supplies', 'Dietary Manager', 9200.00, 8880.00, 'On Target', '2026-04-26')",
            "INSERT INTO budget_items (category, owner, monthly_budget, actual_spend, status, next_review) VALUES ('Housekeeping supplies', 'Environmental Services', 5400.00, 5715.00, 'Watch', '2026-04-25')"
        };
        if (!executeAll(budgetItems)) return false;
    }

    if (tableIsEmpty("compliance_items")) {
        const QStringList complianceItems = {
            "INSERT INTO compliance_items (area, item_name, owner, due_date, status) VALUES ('Licensure', 'Administrator license renewal packet', 'Administrator', '2026-04-26', 'Due Soon')",
            "INSERT INTO compliance_items (area, item_name, owner, due_date, status) VALUES ('Life Safety', 'Quarterly fire drill summary sign-off', 'Maintenance Director', '2026-04-24', 'Due Soon')",
            "INSERT INTO compliance_items (area, item_name, owner, due_date, status) VALUES ('HR', 'Background check audit validation', 'HR / Payroll', '2026-04-29', 'In Progress')",
            "INSERT INTO compliance_items (area, item_name, owner, due_date, status) VALUES ('Clinical', 'Mock survey follow-up evidence upload', 'DON', '2026-04-23', 'Overdue')"
        };
        if (!executeAll(complianceItems)) return false;
    }

    if (tableIsEmpty("staffing_changes")) {
        const QStringList staffingChanges = {
            "INSERT INTO staffing_changes (change_date, department, shift_name, change_type, position_name, employee_name, impact_level, status, notes) VALUES ('2026-04-20', 'Nursing', 'Day', 'Call Off', 'CNA', 'Open Shift', 'High', 'Open', 'Hall 200 day shift needs replacement coverage before 6:30 AM.')",
            "INSERT INTO staffing_changes (change_date, department, shift_name, change_type, position_name, employee_name, impact_level, status, notes) VALUES ('2026-04-20', 'Nursing', 'Evening', 'Schedule Adjustment', 'LPN', 'Taylor Brooks', 'Medium', 'Planned', 'Adjusted start time to cover med-pass overlap during admission intake.')",
            "INSERT INTO staffing_changes (change_date, department, shift_name, change_type, position_name, employee_name, impact_level, status, notes) VALUES ('2026-04-21', 'Environmental Services', 'Day', 'Agency Coverage', 'Housekeeper', 'Agency Pool', 'Medium', 'Confirmed', 'Weekend turnover coverage added for isolation discharge room reset.')",
            "INSERT INTO staffing_changes (change_date, department, shift_name, change_type, position_name, employee_name, impact_level, status, notes) VALUES ('2026-04-21', 'Dietary', 'Weekend', 'Open Shift', 'Cook', 'Open Shift', 'High', 'Open', 'Weekend cook opening remains unfilled and may affect tray timing.')"
        };
        if (!executeAll(staffingChanges)) return false;
    }

    if (tableIsEmpty("huddle_items")) {
        const QStringList huddleItems = {
            "INSERT INTO huddle_items (huddle_date, shift_name, department, topic, owner, priority, status, notes) VALUES ('2026-04-20', 'Morning', 'Nursing', 'High-acuity hallway assignment coverage', 'DON', 'High', 'Open', 'Rebalance assignments before lunch and confirm backup CNA coverage.')",
            "INSERT INTO huddle_items (huddle_date, shift_name, department, topic, owner, priority, status, notes) VALUES ('2026-04-20', 'Morning', 'Admissions', 'Pending hospital referral packet missing therapy notes', 'Admissions Director', 'Medium', 'In Progress', 'Follow up with referral source and update morning stand-up board.')",
            "INSERT INTO huddle_items (huddle_date, shift_name, department, topic, owner, priority, status, notes) VALUES ('2026-04-19', 'Weekend', 'Environmental Services', 'Room turnover behind schedule for planned admit', 'EVS Lead', 'High', 'Escalated', 'Need turnover completion time and readiness confirmation for admissions.')"
        };
        if (!executeAll(huddleItems)) return false;
    }

    if (tableIsEmpty("quality_measures")) {
        const QStringList qualityMeasures = {
            "INSERT INTO quality_measures (measure_name, measure_set, owner, current_value, target_value, next_review, status, notes) VALUES ('Falls with Major Injury', 'CMS Quality Measure', 'DON', 1.90, 1.20, '2026-04-23', 'Off Target', 'Recent spike tied to two high-risk returns and alarm-response consistency on evening shift.')",
            "INSERT INTO quality_measures (measure_name, measure_set, owner, current_value, target_value, next_review, status, notes) VALUES ('30-Day Rehospitalization Rate', 'Rehospitalization', 'Administrator', 18.50, 15.00, '2026-04-24', 'Watch', 'Monitor CHF and sepsis readmit cohort; add 72-hour chart audit review.')",
            "INSERT INTO quality_measures (measure_name, measure_set, owner, current_value, target_value, next_review, status, notes) VALUES ('UTI Long-Stay Rate', 'CMS Quality Measure', 'Infection Preventionist', 2.10, 2.50, '2026-04-28', 'On Target', 'Continue hydration rounds and catheter necessity review.')",
            "INSERT INTO quality_measures (measure_name, measure_set, owner, current_value, target_value, next_review, status, notes) VALUES ('Weight Loss >5%', 'Clinical Risk', 'Dietary Manager', 7.40, 5.00, '2026-04-22', 'Off Target', 'Flag high-risk residents for interdisciplinary nutrition review and reweigh validation.')"
        };
        if (!executeAll(qualityMeasures)) return false;
    }


    if (tableIsEmpty("training_items")) {
        const QStringList trainingItems = {
            "INSERT INTO training_items (training_area, employee_name, role_name, due_date, status, notes) VALUES ('CNA Annual In-Service', 'Jordan Hayes', 'CNA', '2026-04-24', 'Due Soon', 'Needs annual dementia and abuse-prevention in-service completion before payroll close.')",
            "INSERT INTO training_items (training_area, employee_name, role_name, due_date, status, notes) VALUES ('Administrator CEU Tracking', 'Morgan Reed', 'Administrator', '2026-04-30', 'In Progress', 'Verify CEU certificates are uploaded and renewal packet support documents are complete.')",
            "INSERT INTO training_items (training_area, employee_name, role_name, due_date, status, notes) VALUES ('Medication Cart Competency', 'Taylor Brooks', 'LPN', '2026-04-22', 'Overdue', 'Observed workflow variance requires repeat med-pass competency and coach-back.')",
            "INSERT INTO training_items (training_area, employee_name, role_name, due_date, status, notes) VALUES ('Infection Prevention Orientation', 'Casey Bennett', 'Housekeeper', '2026-04-23', 'Scheduled', 'New-hire orientation block includes isolation-room turnover and PPE sequence review.')"
        };
        if (!executeAll(trainingItems)) return false;
    }

    if (tableIsEmpty("emergency_preparedness_items")) {
        const QStringList preparednessItems = {
            "INSERT INTO emergency_preparedness_items (drill_date, scenario_name, owner, next_drill_date, status, notes) VALUES ('2026-04-10', 'Severe Weather Shelter-in-Place Drill', 'Administrator', '2026-04-25', 'Due Soon', 'Need command-center assignment sheet reviewed and backup communication tree validated.')",
            "INSERT INTO emergency_preparedness_items (drill_date, scenario_name, owner, next_drill_date, status, notes) VALUES ('2026-03-28', 'Generator Load and Fuel Continuity Check', 'Maintenance Director', '2026-04-21', 'Overdue', 'Generator runtime log needs completion and vendor fuel-delivery contingency must be confirmed.')",
            "INSERT INTO emergency_preparedness_items (drill_date, scenario_name, owner, next_drill_date, status, notes) VALUES ('2026-04-18', 'Missing Resident Elopement Tabletop', 'DON', '2026-05-02', 'Scheduled', 'Weekend supervisors to review search assignments, door-alarm escalation, and family notification steps.')",
            "INSERT INTO emergency_preparedness_items (drill_date, scenario_name, owner, next_drill_date, status, notes) VALUES ('2026-04-05', 'Evacuation Transportation Roster Audit', 'Social Services', '2026-04-27', 'In Progress', 'Verify stretcher-capable transport list, oxygen vendor support, and receiving-facility contact packets.')"
        };
        if (!executeAll(preparednessItems)) return false;
    }

    if (tableIsEmpty("infection_control_items")) {
        const QStringList infectionItems = {
            "INSERT INTO infection_control_items (event_date, area_name, owner, next_review, status, notes) VALUES ('2026-04-20', 'Isolation PPE Audit', 'Infection Preventionist', '2026-04-22', 'Open', 'Observed doffing-sequence drift on one hall; re-education and spot-check validation needed.')",
            "INSERT INTO infection_control_items (event_date, area_name, owner, next_review, status, notes) VALUES ('2026-04-19', 'Respiratory Symptom Line List Review', 'DON', '2026-04-21', 'Urgent', 'Three new respiratory symptom reports require provider-notification confirmation and exposure review.')",
            "INSERT INTO infection_control_items (event_date, area_name, owner, next_review, status, notes) VALUES ('2026-04-18', 'Antibiotic Stewardship Review', 'Medical Records', '2026-04-24', 'In Progress', 'Verify stop dates and diagnosis support for current antibiotic starts before committee review.')",
            "INSERT INTO infection_control_items (event_date, area_name, owner, next_review, status, notes) VALUES ('2026-04-17', 'Hand Hygiene Observation Trend', 'Unit Manager', '2026-04-23', 'Watch', 'Evening shift compliance dipped below target; coaching and repeat observations scheduled.')"
        };
        if (!executeAll(infectionItems)) return false;
    }

    return true;
}

int DatabaseManager::currentCensus() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM residents WHERE status = 'Current'");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::bedCapacity() const {
    QSqlQuery query(database_);
    query.exec("SELECT bed_capacity FROM facility_info WHERE id = 1");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::openIncidents() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM incidents WHERE status = 'Open'");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::openTasks() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM tasks WHERE status = 'Open'");
    return query.next() ? query.value(0).toInt() : 0;
}

double DatabaseManager::averageStaffHoursPerResidentDay() const {
    QSqlQuery query(database_);
    query.exec("SELECT AVG(hprd) FROM staffing");
    return query.next() ? query.value(0).toDouble() : 0.0;
}

int DatabaseManager::surveyItemsAtRisk() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM survey_items WHERE readiness_status IN ('At Risk', 'Needs Review')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::pendingAdmissions() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM admissions WHERE status IN ('Pending', 'Insurance Review', 'Hospital Hold')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::plannedDischarges() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM admissions WHERE status = 'Planned Discharge'");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::highPriorityTasks() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM tasks WHERE status = 'Open' AND priority IN ('Critical', 'High')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::agencyShifts() const {
    QSqlQuery query(database_);
    query.exec("SELECT COALESCE(SUM(agency), 0) FROM staffing");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::activeQapiProjects() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM qapi_projects WHERE status = 'Active'");
    return query.next() ? query.value(0).toInt() : 0;
}

double DatabaseManager::totalOvertimeHours() const {
    QSqlQuery query(database_);
    query.exec("SELECT COALESCE(SUM(overtime_hours), 0) FROM staffing");
    return query.next() ? query.value(0).toDouble() : 0.0;
}

double DatabaseManager::monthlyLaborBudgetVariance() const {
    QSqlQuery query(database_);
    query.exec("SELECT COALESCE(SUM(actual_spend - monthly_budget), 0) FROM budget_items");
    return query.next() ? query.value(0).toDouble() : 0.0;
}

int DatabaseManager::budgetItemsAtRisk() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM budget_items WHERE status IN ('At Risk', 'Watch')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::complianceItemsDueSoon() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM compliance_items WHERE status IN ('Due Soon', 'Overdue', 'In Progress')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::pendingStaffingChanges() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM staffing_changes WHERE status IN ('Open', 'Planned')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::openHuddleItems() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM huddle_items WHERE status IN ('Open', 'In Progress', 'Escalated')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::qualityMeasuresOffTarget() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM quality_measures WHERE status IN ('Off Target', 'Watch')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::trainingItemsDueSoon() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM training_items WHERE status IN ('Due Soon', 'Overdue', 'In Progress', 'Scheduled')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::emergencyPreparednessItemsDueSoon() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM emergency_preparedness_items WHERE status IN ('Due Soon', 'Overdue', 'In Progress', 'Scheduled')");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::infectionControlItemsOpen() const {
    QSqlQuery query(database_);
    query.exec("SELECT COUNT(*) FROM infection_control_items WHERE status IN ('Open', 'Watch', 'In Progress', 'Urgent')");
    return query.next() ? query.value(0).toInt() : 0;
}

QString DatabaseManager::facilityName() const {
    QSqlQuery query(database_);
    query.exec("SELECT facility_name FROM facility_info WHERE id = 1");
    return query.next() ? query.value(0).toString() : QStringLiteral("LTC Facility");
}

QStringList DatabaseManager::dashboardFocusItems() const {
    QStringList items;

    QSqlQuery tasksQuery(database_);
    tasksQuery.exec("SELECT title, due_date, owner, priority FROM tasks WHERE status = 'Open' ORDER BY CASE priority WHEN 'Critical' THEN 1 WHEN 'High' THEN 2 WHEN 'Medium' THEN 3 ELSE 4 END, due_date LIMIT 3");
    while (tasksQuery.next()) {
        items << QString("Task: %1 | Owner: %2 | Priority: %3 | Due: %4")
                     .arg(tasksQuery.value(0).toString(),
                          tasksQuery.value(2).toString(),
                          tasksQuery.value(3).toString(),
                          tasksQuery.value(1).toString());
    }

    QSqlQuery incidentQuery(database_);
    incidentQuery.exec("SELECT resident_name, severity, event_date FROM incidents WHERE status = 'Open' ORDER BY CASE severity WHEN 'Immediate Jeopardy' THEN 1 WHEN 'High' THEN 2 WHEN 'Moderate' THEN 3 ELSE 4 END, event_date DESC LIMIT 2");
    while (incidentQuery.next()) {
        items << QString("Incident: %1 | Severity: %2 | Date: %3")
                     .arg(incidentQuery.value(0).toString(),
                          incidentQuery.value(1).toString(),
                          incidentQuery.value(2).toString());
    }

    QSqlQuery surveyQuery(database_);
    surveyQuery.exec("SELECT item_name, owner, next_review FROM survey_items WHERE readiness_status IN ('At Risk', 'Needs Review') ORDER BY next_review LIMIT 2");
    while (surveyQuery.next()) {
        items << QString("Survey: %1 | Owner: %2 | Review: %3")
                     .arg(surveyQuery.value(0).toString(),
                          surveyQuery.value(1).toString(),
                          surveyQuery.value(2).toString());
    }

    QSqlQuery qapiQuery(database_);
    qapiQuery.exec("SELECT topic, owner, next_review FROM qapi_projects WHERE status = 'Active' ORDER BY next_review LIMIT 2");
    while (qapiQuery.next()) {
        items << QString("QAPI: %1 | Owner: %2 | Review: %3")
                     .arg(qapiQuery.value(0).toString(),
                          qapiQuery.value(1).toString(),
                          qapiQuery.value(2).toString());
    }

    QSqlQuery budgetQuery(database_);
    budgetQuery.exec("SELECT category, owner, actual_spend - monthly_budget, next_review FROM budget_items WHERE status IN ('At Risk', 'Watch') ORDER BY CASE status WHEN 'At Risk' THEN 1 ELSE 2 END, next_review LIMIT 2");
    while (budgetQuery.next()) {
        items << QString("Budget: %1 | Owner: %2 | Variance: $%3 | Review: %4")
                     .arg(budgetQuery.value(0).toString(),
                          budgetQuery.value(1).toString(),
                          QString::number(budgetQuery.value(2).toDouble(), 'f', 0),
                          budgetQuery.value(3).toString());
    }

    QSqlQuery complianceQuery(database_);
    complianceQuery.exec("SELECT area, item_name, owner, due_date, status FROM compliance_items WHERE status IN ('Due Soon', 'Overdue', 'In Progress') ORDER BY CASE status WHEN 'Overdue' THEN 1 WHEN 'Due Soon' THEN 2 ELSE 3 END, due_date LIMIT 3");
    while (complianceQuery.next()) {
        items << QString("Compliance: %1 | %2 | Owner: %3 | Due: %4 | Status: %5")
                     .arg(complianceQuery.value(0).toString(),
                          complianceQuery.value(1).toString(),
                          complianceQuery.value(2).toString(),
                          complianceQuery.value(3).toString(),
                          complianceQuery.value(4).toString());
    }

    QSqlQuery staffingChangeQuery(database_);
    staffingChangeQuery.exec("SELECT department, shift_name, change_type, impact_level, status FROM staffing_changes WHERE status IN ('Open', 'Planned') ORDER BY CASE impact_level WHEN 'Critical' THEN 1 WHEN 'High' THEN 2 WHEN 'Medium' THEN 3 ELSE 4 END, change_date LIMIT 3");
    while (staffingChangeQuery.next()) {
        items << QString("Staffing Change: %1 | Shift: %2 | Type: %3 | Impact: %4 | Status: %5")
                     .arg(staffingChangeQuery.value(0).toString(),
                          staffingChangeQuery.value(1).toString(),
                          staffingChangeQuery.value(2).toString(),
                          staffingChangeQuery.value(3).toString(),
                          staffingChangeQuery.value(4).toString());
    }

    QSqlQuery qualityQuery(database_);
    qualityQuery.exec("SELECT measure_name, owner, next_review, status FROM quality_measures WHERE status IN ('Off Target', 'Watch') ORDER BY CASE status WHEN 'Off Target' THEN 1 ELSE 2 END, next_review LIMIT 3");
    while (qualityQuery.next()) {
        items << QString("Quality: %1 | Owner: %2 | Review: %3 | Status: %4")
                     .arg(qualityQuery.value(0).toString(),
                          qualityQuery.value(1).toString(),
                          qualityQuery.value(2).toString(),
                          qualityQuery.value(3).toString());
    }


    QSqlQuery trainingQuery(database_);
    trainingQuery.exec("SELECT training_area, employee_name, due_date, status FROM training_items WHERE status IN ('Due Soon', 'Overdue', 'In Progress', 'Scheduled') ORDER BY CASE status WHEN 'Overdue' THEN 1 WHEN 'Due Soon' THEN 2 WHEN 'In Progress' THEN 3 ELSE 4 END, due_date LIMIT 3");
    while (trainingQuery.next()) {
        items << QString("Training: %1 | Employee: %2 | Due: %3 | Status: %4")
                     .arg(trainingQuery.value(0).toString(),
                          trainingQuery.value(1).toString(),
                          trainingQuery.value(2).toString(),
                          trainingQuery.value(3).toString());
    }

    QSqlQuery preparednessQuery(database_);
    preparednessQuery.exec("SELECT scenario_name, owner, next_drill_date, status FROM emergency_preparedness_items WHERE status IN ('Due Soon', 'Overdue', 'In Progress', 'Scheduled') ORDER BY CASE status WHEN 'Overdue' THEN 1 WHEN 'Due Soon' THEN 2 WHEN 'In Progress' THEN 3 ELSE 4 END, next_drill_date LIMIT 3");
    while (preparednessQuery.next()) {
        items << QString("Preparedness: %1 | Owner: %2 | Next Drill: %3 | Status: %4")
                     .arg(preparednessQuery.value(0).toString(),
                          preparednessQuery.value(1).toString(),
                          preparednessQuery.value(2).toString(),
                          preparednessQuery.value(3).toString());
    }

    if (items.isEmpty()) {
        items << "No critical operational follow-up items found.";
    }
    return items;
}

QSqlQueryModel* DatabaseManager::createResidentsModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT name AS Resident, payer_source AS Payer, room AS Room, acuity AS Acuity, admit_date AS 'Admit Date', status AS Status FROM residents ORDER BY room", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createStaffingModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT department AS Department, scheduled AS Scheduled, worked AS Worked, agency AS Agency, overtime_hours AS 'OT Hours', hprd AS HPRD FROM staffing ORDER BY department", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createTasksModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT title AS Task, owner AS Owner, priority AS Priority, due_date AS 'Due Date', status AS Status FROM tasks ORDER BY CASE priority WHEN 'Critical' THEN 1 WHEN 'High' THEN 2 WHEN 'Medium' THEN 3 ELSE 4 END, due_date", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createIncidentsModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT event_date AS Date, resident_name AS Resident, severity AS Severity, description AS Description, status AS Status FROM incidents ORDER BY event_date DESC", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createSurveyModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT domain_name AS Domain, item_name AS Item, owner AS Owner, readiness_status AS Status, next_review AS 'Next Review' FROM survey_items ORDER BY CASE readiness_status WHEN 'At Risk' THEN 1 WHEN 'Needs Review' THEN 2 ELSE 3 END, next_review", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createAdmissionsModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT resident_name AS Resident, referral_source AS Referral, payer_source AS Payer, expected_date AS 'Expected Date', status AS Status FROM admissions ORDER BY expected_date", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createQapiModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT topic AS Topic, owner AS Owner, source AS Trigger, stage AS Stage, next_review AS 'Next Review', status AS Status, aim_statement AS 'Aim Statement' FROM qapi_projects ORDER BY next_review, topic", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createBudgetModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT category AS Category, owner AS Owner, monthly_budget AS 'Budget', actual_spend AS 'Actual', (actual_spend - monthly_budget) AS 'Variance', status AS Status, next_review AS 'Next Review' FROM budget_items ORDER BY CASE status WHEN 'At Risk' THEN 1 WHEN 'Watch' THEN 2 ELSE 3 END, next_review, category", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createComplianceModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT area AS Area, item_name AS Requirement, owner AS Owner, due_date AS 'Due Date', status AS Status FROM compliance_items ORDER BY CASE status WHEN 'Overdue' THEN 1 WHEN 'Due Soon' THEN 2 WHEN 'In Progress' THEN 3 ELSE 4 END, due_date, area", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createStaffingChangesModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT change_date AS 'Change Date', department AS Department, shift_name AS Shift, change_type AS 'Change Type', position_name AS Position, employee_name AS Employee, impact_level AS Impact, status AS Status, notes AS Notes FROM staffing_changes ORDER BY CASE impact_level WHEN 'Critical' THEN 1 WHEN 'High' THEN 2 WHEN 'Medium' THEN 3 ELSE 4 END, change_date DESC, department", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createHuddleItemsModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT huddle_date AS 'Huddle Date', shift_name AS Shift, department AS Department, topic AS Topic, owner AS Owner, priority AS Priority, status AS Status, notes AS Notes FROM huddle_items ORDER BY huddle_date DESC, CASE priority WHEN 'Critical' THEN 1 WHEN 'High' THEN 2 WHEN 'Medium' THEN 3 ELSE 4 END, department", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createQualityMeasuresModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT measure_name AS 'Measure', measure_set AS 'Measure Set', owner AS Owner, current_value AS 'Current', target_value AS 'Target', next_review AS 'Next Review', status AS Status, notes AS Notes FROM quality_measures ORDER BY CASE status WHEN 'Off Target' THEN 1 WHEN 'Watch' THEN 2 ELSE 3 END, next_review, measure_name", database_);
    return model;
}


QSqlQueryModel* DatabaseManager::createTrainingModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT training_area AS 'Training Area', employee_name AS Employee, role_name AS Role, due_date AS 'Due Date', status AS Status, notes AS Notes FROM training_items ORDER BY CASE status WHEN 'Overdue' THEN 1 WHEN 'Due Soon' THEN 2 WHEN 'In Progress' THEN 3 WHEN 'Scheduled' THEN 4 ELSE 5 END, due_date, training_area", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createEmergencyPreparednessModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT drill_date AS 'Last Drill', scenario_name AS Scenario, owner AS Owner, next_drill_date AS 'Next Drill', status AS Status, notes AS Notes FROM emergency_preparedness_items ORDER BY CASE status WHEN 'Overdue' THEN 1 WHEN 'Due Soon' THEN 2 WHEN 'In Progress' THEN 3 WHEN 'Scheduled' THEN 4 ELSE 5 END, next_drill_date, scenario_name", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createInfectionControlModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT event_date AS 'Event Date', area_name AS 'Area / Topic', owner AS Owner, next_review AS 'Next Review', status AS Status, notes AS Notes FROM infection_control_items ORDER BY CASE status WHEN 'Urgent' THEN 1 WHEN 'Open' THEN 2 WHEN 'Watch' THEN 3 WHEN 'In Progress' THEN 4 ELSE 5 END, next_review, area_name", database_);
    return model;
}

bool DatabaseManager::addTask(const QString& title, const QString& owner, const QString& priority, const QString& dueDate) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO tasks (title, owner, priority, due_date, status) VALUES (?, ?, ?, ?, 'Open')");
    query.addBindValue(title);
    query.addBindValue(owner);
    query.addBindValue(priority);
    query.addBindValue(dueDate);
    return query.exec();
}

bool DatabaseManager::addIncident(const QString& eventDate, const QString& residentName, const QString& severity, const QString& description) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO incidents (event_date, resident_name, severity, description, status) VALUES (?, ?, ?, ?, 'Open')");
    query.addBindValue(eventDate);
    query.addBindValue(residentName);
    query.addBindValue(severity);
    query.addBindValue(description);
    return query.exec();
}

bool DatabaseManager::addAdmission(const QString& residentName,
                                   const QString& referralSource,
                                   const QString& payerSource,
                                   const QString& expectedDate,
                                   const QString& status) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO admissions (resident_name, referral_source, payer_source, expected_date, status) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(residentName);
    query.addBindValue(referralSource);
    query.addBindValue(payerSource);
    query.addBindValue(expectedDate);
    query.addBindValue(status);
    return query.exec();
}

bool DatabaseManager::addQapiProject(const QString& topic,
                                     const QString& owner,
                                     const QString& source,
                                     const QString& stage,
                                     const QString& nextReview,
                                     const QString& aimStatement) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO qapi_projects (topic, owner, source, stage, next_review, aim_statement, status) VALUES (?, ?, ?, ?, ?, ?, 'Active')");
    query.addBindValue(topic);
    query.addBindValue(owner);
    query.addBindValue(source);
    query.addBindValue(stage);
    query.addBindValue(nextReview);
    query.addBindValue(aimStatement);
    return query.exec();
}

bool DatabaseManager::addBudgetItem(const QString& category,
                                    const QString& owner,
                                    double monthlyBudget,
                                    double actualSpend,
                                    const QString& status,
                                    const QString& nextReview) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO budget_items (category, owner, monthly_budget, actual_spend, status, next_review) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(category);
    query.addBindValue(owner);
    query.addBindValue(monthlyBudget);
    query.addBindValue(actualSpend);
    query.addBindValue(status);
    query.addBindValue(nextReview);
    return query.exec();
}

bool DatabaseManager::addComplianceItem(const QString& area,
                                        const QString& itemName,
                                        const QString& owner,
                                        const QString& dueDate,
                                        const QString& status) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO compliance_items (area, item_name, owner, due_date, status) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(area);
    query.addBindValue(itemName);
    query.addBindValue(owner);
    query.addBindValue(dueDate);
    query.addBindValue(status);
    return query.exec();
}

bool DatabaseManager::addStaffingChange(const QString& changeDate,
                                        const QString& department,
                                        const QString& shiftName,
                                        const QString& changeType,
                                        const QString& positionName,
                                        const QString& employeeName,
                                        const QString& impactLevel,
                                        const QString& status,
                                        const QString& notes) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO staffing_changes (change_date, department, shift_name, change_type, position_name, employee_name, impact_level, status, notes) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(changeDate);
    query.addBindValue(department);
    query.addBindValue(shiftName);
    query.addBindValue(changeType);
    query.addBindValue(positionName);
    query.addBindValue(employeeName);
    query.addBindValue(impactLevel);
    query.addBindValue(status);
    query.addBindValue(notes);
    return query.exec();
}

bool DatabaseManager::addHuddleItem(const QString& huddleDate,
                                    const QString& shiftName,
                                    const QString& department,
                                    const QString& topic,
                                    const QString& owner,
                                    const QString& priority,
                                    const QString& status,
                                    const QString& notes) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO huddle_items (huddle_date, shift_name, department, topic, owner, priority, status, notes) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(huddleDate);
    query.addBindValue(shiftName);
    query.addBindValue(department);
    query.addBindValue(topic);
    query.addBindValue(owner);
    query.addBindValue(priority);
    query.addBindValue(status);
    query.addBindValue(notes);
    return query.exec();
}

bool DatabaseManager::addQualityMeasure(const QString& measureName,
                                        const QString& measureSet,
                                        const QString& owner,
                                        double currentValue,
                                        double targetValue,
                                        const QString& nextReview,
                                        const QString& status,
                                        const QString& notes) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO quality_measures (measure_name, measure_set, owner, current_value, target_value, next_review, status, notes) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(measureName);
    query.addBindValue(measureSet);
    query.addBindValue(owner);
    query.addBindValue(currentValue);
    query.addBindValue(targetValue);
    query.addBindValue(nextReview);
    query.addBindValue(status);
    query.addBindValue(notes);
    return query.exec();
}




bool DatabaseManager::addTrainingItem(const QString& trainingArea,
                                      const QString& employeeName,
                                      const QString& roleName,
                                      const QString& dueDate,
                                      const QString& status,
                                      const QString& notes) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO training_items (training_area, employee_name, role_name, due_date, status, notes) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(trainingArea);
    query.addBindValue(employeeName);
    query.addBindValue(roleName);
    query.addBindValue(dueDate);
    query.addBindValue(status);
    query.addBindValue(notes);
    return query.exec();
}

bool DatabaseManager::addEmergencyPreparednessItem(const QString& drillDate,
                                                   const QString& scenarioName,
                                                   const QString& owner,
                                                   const QString& nextDrillDate,
                                                   const QString& status,
                                                   const QString& notes) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO emergency_preparedness_items (drill_date, scenario_name, owner, next_drill_date, status, notes) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(drillDate);
    query.addBindValue(scenarioName);
    query.addBindValue(owner);
    query.addBindValue(nextDrillDate);
    query.addBindValue(status);
    query.addBindValue(notes);
    return query.exec();
}


bool DatabaseManager::addInfectionControlItem(const QString& eventDate,
                                              const QString& areaName,
                                              const QString& owner,
                                              const QString& nextReview,
                                              const QString& status,
                                              const QString& notes) {
    QSqlQuery query(database_);
    query.prepare("INSERT INTO infection_control_items (event_date, area_name, owner, next_review, status, notes) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(eventDate);
    query.addBindValue(areaName);
    query.addBindValue(owner);
    query.addBindValue(nextReview);
    query.addBindValue(status);
    query.addBindValue(notes);
    return query.exec();
}
