#include "DatabaseManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDate>

DatabaseManager::DatabaseManager() {}
DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) m_db.close();
}

bool DatabaseManager::initialize() {
    const QString dbPath = QDir(QCoreApplication::applicationDirPath()).filePath("ltc_admin_dashboard_v23.db");
    if (QSqlDatabase::contains("ltc_connection")) {
        m_db = QSqlDatabase::database("ltc_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "ltc_connection");
        m_db.setDatabaseName(dbPath);
    }
    if (!m_db.open()) return false;
    return createTables() && seedData();
}

bool DatabaseManager::createTables() {
    const QStringList ddl = {
        "CREATE TABLE IF NOT EXISTS residents (id INTEGER PRIMARY KEY AUTOINCREMENT, resident_name TEXT, room TEXT, payer TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS admissions (id INTEGER PRIMARY KEY AUTOINCREMENT, resident_name TEXT, referral_source TEXT, planned_date TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS staffing_assignments (id INTEGER PRIMARY KEY AUTOINCREMENT, work_date TEXT, department TEXT, shift_name TEXT, role_name TEXT, employee_name TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS staffing_minimums (id INTEGER PRIMARY KEY AUTOINCREMENT, department TEXT, shift_name TEXT, role_name TEXT, minimum_required INTEGER)",
        "CREATE TABLE IF NOT EXISTS staffing_changes (id INTEGER PRIMARY KEY AUTOINCREMENT, change_date TEXT, department TEXT, shift_name TEXT, change_type TEXT, position_name TEXT, employee_name TEXT, impact_level TEXT, status TEXT, notes TEXT)",
        "CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, due_date TEXT, owner TEXT, task_name TEXT, priority TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS pips (id INTEGER PRIMARY KEY AUTOINCREMENT, project_name TEXT, owner TEXT, status TEXT, next_step TEXT)",
        "CREATE TABLE IF NOT EXISTS budget_items (id INTEGER PRIMARY KEY AUTOINCREMENT, item_name TEXT, department TEXT, variance TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS compliance_items (id INTEGER PRIMARY KEY AUTOINCREMENT, item_name TEXT, due_date TEXT, owner TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS huddle_items (id INTEGER PRIMARY KEY AUTOINCREMENT, huddle_date TEXT, shift_name TEXT, department TEXT, topic TEXT, owner TEXT, priority TEXT, status TEXT, notes TEXT)",
        "CREATE TABLE IF NOT EXISTS incidents (id INTEGER PRIMARY KEY AUTOINCREMENT, incident_date TEXT, resident_name TEXT, incident_type TEXT, severity TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS survey_items (id INTEGER PRIMARY KEY AUTOINCREMENT, focus_area TEXT, owner TEXT, risk_level TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS quality_measures (id INTEGER PRIMARY KEY AUTOINCREMENT, measure_name TEXT, current_value TEXT, target_value TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS managed_care_items (id INTEGER PRIMARY KEY AUTOINCREMENT, resident_name TEXT, payer TEXT, item_name TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS credentialing_items (id INTEGER PRIMARY KEY AUTOINCREMENT, employee_name TEXT, item_name TEXT, due_date TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS preparedness_items (id INTEGER PRIMARY KEY AUTOINCREMENT, item_name TEXT, due_date TEXT, owner TEXT, status TEXT)",
        "CREATE TABLE IF NOT EXISTS infection_control_items (id INTEGER PRIMARY KEY AUTOINCREMENT, item_name TEXT, owner TEXT, status TEXT, notes TEXT)",
        "CREATE TABLE IF NOT EXISTS grievances (id INTEGER PRIMARY KEY AUTOINCREMENT, report_date TEXT, category TEXT, resident_or_family TEXT, owner TEXT, priority TEXT, status TEXT, summary TEXT)",
        "CREATE TABLE IF NOT EXISTS environmental_rounds (id INTEGER PRIMARY KEY AUTOINCREMENT, round_date TEXT, area_name TEXT, issue_name TEXT, owner TEXT, priority TEXT, status TEXT, notes TEXT)",
        "CREATE TABLE IF NOT EXISTS bed_board (id INTEGER PRIMARY KEY AUTOINCREMENT, room_number TEXT, bed_status TEXT, resident_name TEXT, pending_action TEXT, owner TEXT, status TEXT, notes TEXT)",
        "CREATE TABLE IF NOT EXISTS transport_items (id INTEGER PRIMARY KEY AUTOINCREMENT, appointment_date TEXT, resident_name TEXT, appointment_type TEXT, destination TEXT, transport_mode TEXT, owner TEXT, status TEXT, notes TEXT)",
        "CREATE TABLE IF NOT EXISTS pharmacy_items (id INTEGER PRIMARY KEY AUTOINCREMENT, review_date TEXT, resident_name TEXT, item_name TEXT, owner TEXT, priority TEXT, status TEXT, notes TEXT)"
    };
    return executeAll(ddl);
}

bool DatabaseManager::executeAll(const QStringList& statements) const {
    for (const QString& stmt : statements) {
        QSqlQuery q(m_db);
        if (!q.exec(stmt)) return false;
    }
    return true;
}

bool DatabaseManager::tableIsEmpty(const QString& tableName) const {
    QSqlQuery q(m_db);
    q.exec(QString("SELECT COUNT(*) FROM %1").arg(tableName));
    return q.next() && q.value(0).toInt() == 0;
}

bool DatabaseManager::seedData() {
    if (tableIsEmpty("residents")) {
        if (!executeAll({
            "INSERT INTO residents (resident_name, room, payer, status) VALUES ('Martha Lane', '102A', 'Medicare A', 'Current')",
            "INSERT INTO residents (resident_name, room, payer, status) VALUES ('James Hill', '114B', 'Managed Care', 'Current')",
            "INSERT INTO residents (resident_name, room, payer, status) VALUES ('Evelyn Cross', '209A', 'Medicaid', 'Current')"
        })) return false;
    }
    if (tableIsEmpty("admissions")) {
        if (!executeAll({
            "INSERT INTO admissions (resident_name, referral_source, planned_date, status) VALUES ('New Referral - Ortho', 'Regional Hospital', '2026-04-21', 'Pending')",
            "INSERT INTO admissions (resident_name, referral_source, planned_date, status) VALUES ('New Referral - Rehab', 'Mercy Discharge Planner', '2026-04-22', 'Accepted')"
        })) return false;
    }
    if (tableIsEmpty("staffing_minimums")) {
        if (!executeAll({
            "INSERT INTO staffing_minimums (department, shift_name, role_name, minimum_required) VALUES ('Nursing', 'Day', 'RN/LPN', 2)",
            "INSERT INTO staffing_minimums (department, shift_name, role_name, minimum_required) VALUES ('Nursing', 'Day', 'CNA', 4)",
            "INSERT INTO staffing_minimums (department, shift_name, role_name, minimum_required) VALUES ('Nursing', 'Evening', 'RN/LPN', 1)",
            "INSERT INTO staffing_minimums (department, shift_name, role_name, minimum_required) VALUES ('Nursing', 'Evening', 'CNA', 3)",
            "INSERT INTO staffing_minimums (department, shift_name, role_name, minimum_required) VALUES ('Nursing', 'Night', 'RN/LPN', 1)",
            "INSERT INTO staffing_minimums (department, shift_name, role_name, minimum_required) VALUES ('Nursing', 'Night', 'CNA', 2)",
            "INSERT INTO staffing_minimums (department, shift_name, role_name, minimum_required) VALUES ('Dietary', 'Evening', 'Cook', 1)",
            "INSERT INTO staffing_minimums (department, shift_name, role_name, minimum_required) VALUES ('Environmental Services', 'Day', 'Housekeeper', 2)"
        })) return false;
    }
    if (tableIsEmpty("staffing_assignments")) {
        if (!executeAll({
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Day', 'RN/LPN', 'Jamie Brooks', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Day', 'RN/LPN', 'Alex Carter', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Day', 'CNA', 'Taylor Reed', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Day', 'CNA', 'Morgan Lee', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Day', 'CNA', 'Chris Stone', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Day', 'CNA', 'Open Position', 'Open')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Evening', 'RN/LPN', 'Pat Morgan', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Evening', 'CNA', 'Jordan Blake', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Evening', 'CNA', 'Casey Dean', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Nursing', 'Evening', 'CNA', 'Open Position', 'Open')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Dietary', 'Evening', 'Cook', 'Agency Pool', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Environmental Services', 'Day', 'Housekeeper', 'Robin Allen', 'Filled')",
            "INSERT INTO staffing_assignments (work_date, department, shift_name, role_name, employee_name, status) VALUES ('2026-04-20', 'Environmental Services', 'Day', 'Housekeeper', 'Open Position', 'Open')"
        })) return false;
    }
    if (tableIsEmpty("staffing_changes")) {
        if (!executeAll({
            "INSERT INTO staffing_changes (change_date, department, shift_name, change_type, position_name, employee_name, impact_level, status, notes) VALUES ('2026-04-20', 'Nursing', 'Day', 'Call Off', 'CNA', 'Open Shift', 'High', 'Open', 'Replace before breakfast pass')",
            "INSERT INTO staffing_changes (change_date, department, shift_name, change_type, position_name, employee_name, impact_level, status, notes) VALUES ('2026-04-20', 'Dietary', 'Evening', 'Agency Coverage', 'Cook', 'Agency Pool', 'Medium', 'Confirmed', 'Coverage confirmed for supper service')"
        })) return false;
    }
    if (tableIsEmpty("tasks")) {
        if (!executeAll({
            "INSERT INTO tasks (due_date, owner, task_name, priority, status) VALUES ('2026-04-21', 'Administrator', 'Finalize IDT follow-up list', 'High', 'Open')",
            "INSERT INTO tasks (due_date, owner, task_name, priority, status) VALUES ('2026-04-22', 'Business Office', 'Resolve authorization gap', 'High', 'In Progress')"
        })) return false;
    }
    if (tableIsEmpty("pips")) {
        if (!executeAll({
            "INSERT INTO pips (project_name, owner, status, next_step) VALUES ('Falls Reduction PIP', 'DON', 'Active', 'Review post-fall huddle trends')",
            "INSERT INTO pips (project_name, owner, status, next_step) VALUES ('Weight Loss Monitoring', 'RD', 'Watch', 'Escalate unresolved meal refusal patterns')"
        })) return false;
    }
    if (tableIsEmpty("budget_items")) {
        if (!executeAll({
            "INSERT INTO budget_items (item_name, department, variance, status) VALUES ('Agency nursing spend', 'Nursing', '+12%', 'At Risk')",
            "INSERT INTO budget_items (item_name, department, variance, status) VALUES ('Overtime watch', 'Environmental Services', '+6%', 'Watch')"
        })) return false;
    }
    if (tableIsEmpty("compliance_items")) {
        if (!executeAll({
            "INSERT INTO compliance_items (item_name, due_date, owner, status) VALUES ('Mock survey readiness round', '2026-04-24', 'Administrator', 'Due Soon')",
            "INSERT INTO compliance_items (item_name, due_date, owner, status) VALUES ('Background recheck review', '2026-04-25', 'HR', 'Open')"
        })) return false;
    }
    if (tableIsEmpty("huddle_items")) {
        if (!executeAll({
            "INSERT INTO huddle_items (huddle_date, shift_name, department, topic, owner, priority, status, notes) VALUES ('2026-04-20', 'Morning', 'Admissions', 'Pending packet items', 'Admissions Director', 'High', 'Open', 'Awaiting therapy notes')",
            "INSERT INTO huddle_items (huddle_date, shift_name, department, topic, owner, priority, status, notes) VALUES ('2026-04-20', 'Morning', 'Nursing', 'High-acuity hallway coverage', 'DON', 'High', 'In Progress', 'Review assignment grid by 10 AM')"
        })) return false;
    }
    if (tableIsEmpty("incidents")) {
        if (!executeAll({
            "INSERT INTO incidents (incident_date, resident_name, incident_type, severity, status) VALUES ('2026-04-19', 'James Hill', 'Fall', 'Moderate', 'Review Pending')",
            "INSERT INTO incidents (incident_date, resident_name, incident_type, severity, status) VALUES ('2026-04-20', 'Martha Lane', 'Elopement Risk Alert', 'High', 'Open')"
        })) return false;
    }
    if (tableIsEmpty("survey_items")) {
        if (!executeAll({
            "INSERT INTO survey_items (focus_area, owner, risk_level, status) VALUES ('Infection control tracer binder', 'IP Nurse', 'Medium', 'Open')",
            "INSERT INTO survey_items (focus_area, owner, risk_level, status) VALUES ('Abuse policy staff interview prep', 'Administrator', 'High', 'Open')"
        })) return false;
    }
    if (tableIsEmpty("quality_measures")) {
        if (!executeAll({
            "INSERT INTO quality_measures (measure_name, current_value, target_value, status) VALUES ('30-day rehospitalization', '18.4%', '<= 15.0%', 'Off Target')",
            "INSERT INTO quality_measures (measure_name, current_value, target_value, status) VALUES ('Long-stay UTI rate', '3.2%', '<= 2.5%', 'Watch')"
        })) return false;
    }
    if (tableIsEmpty("managed_care_items")) {
        if (!executeAll({
            "INSERT INTO managed_care_items (resident_name, payer, item_name, status) VALUES ('Evelyn Cross', 'SecureHealth MA', 'Authorization extension', 'At Risk')",
            "INSERT INTO managed_care_items (resident_name, payer, item_name, status) VALUES ('James Hill', 'PrimeCare', 'Clinical update fax', 'Open')"
        })) return false;
    }
    if (tableIsEmpty("credentialing_items")) {
        if (!executeAll({
            "INSERT INTO credentialing_items (employee_name, item_name, due_date, status) VALUES ('Taylor Brooks', 'Medication cart competency', '2026-04-23', 'Due Soon')",
            "INSERT INTO credentialing_items (employee_name, item_name, due_date, status) VALUES ('Morgan Reed', 'Annual in-service', '2026-04-27', 'Open')"
        })) return false;
    }
    if (tableIsEmpty("preparedness_items")) {
        if (!executeAll({
            "INSERT INTO preparedness_items (item_name, due_date, owner, status) VALUES ('Generator load test review', '2026-04-26', 'Maintenance Director', 'Open')",
            "INSERT INTO preparedness_items (item_name, due_date, owner, status) VALUES ('Severe weather drill critique', '2026-04-24', 'Administrator', 'Due Soon')"
        })) return false;
    }
    if (tableIsEmpty("infection_control_items")) {
        if (!executeAll({
            "INSERT INTO infection_control_items (item_name, owner, status, notes) VALUES ('Isolation PPE audit', 'IP Nurse', 'Open', 'Two carts missing gown restock')",
            "INSERT INTO infection_control_items (item_name, owner, status, notes) VALUES ('Antibiotic stewardship review', 'Medical Director', 'Watch', 'Monitor fluoroquinolone use trend')"
        })) return false;
    }
    if (tableIsEmpty("grievances")) {
        if (!executeAll({
            "INSERT INTO grievances (report_date, category, resident_or_family, owner, priority, status, summary) VALUES ('2026-04-20', 'Family Concern', 'Family of Martha Lane', 'Social Services', 'High', 'Open', 'Concern about delayed return call regarding care plan update.')",
            "INSERT INTO grievances (report_date, category, resident_or_family, owner, priority, status, summary) VALUES ('2026-04-19', 'Resident Concern', 'James Hill', 'Administrator', 'Medium', 'In Progress', 'Requested room temperature follow-up and maintenance response.')",
            "INSERT INTO grievances (report_date, category, resident_or_family, owner, priority, status, summary) VALUES ('2026-04-18', 'Service Recovery', 'Family of Evelyn Cross', 'Nursing', 'High', 'Escalated', 'Reported missed update after change in condition.')"
        })) return false;
    }
    if (tableIsEmpty("environmental_rounds")) {
        if (!executeAll({
            "INSERT INTO environmental_rounds (round_date, area_name, issue_name, owner, priority, status, notes) VALUES ('2026-04-20', 'West Hall', 'Handrail repair follow-up', 'Maintenance Director', 'High', 'Open', 'Repair parts on site and completion needed before evening rounds.')",
            "INSERT INTO environmental_rounds (round_date, area_name, issue_name, owner, priority, status, notes) VALUES ('2026-04-20', 'Laundry', 'Dryer exhaust cleaning verification', 'Plant Operations', 'Medium', 'In Progress', 'Vendor verification and log signoff pending.')",
            "INSERT INTO environmental_rounds (round_date, area_name, issue_name, owner, priority, status, notes) VALUES ('2026-04-19', 'Dining Room', 'Floor transition trip-risk observation', 'Housekeeping', 'High', 'Watch', 'Temporary marker in place until permanent repair is scheduled.')"
        })) return false;
    }
    if (tableIsEmpty("bed_board")) {
        if (!executeAll({
            "INSERT INTO bed_board (room_number, bed_status, resident_name, pending_action, owner, status, notes) VALUES ('118A', 'Open', '', 'Room turnover for accepted admission', 'Admissions', 'Open', 'Awaiting EVS final ready check before 3 PM admit.')",
            "INSERT INTO bed_board (room_number, bed_status, resident_name, pending_action, owner, status, notes) VALUES ('122B', 'Hold', 'Recent discharge', 'Family belongings pickup and bed sanitization', 'Social Services', 'In Progress', 'Belongings pickup expected by noon; then release to EVS.')",
            "INSERT INTO bed_board (room_number, bed_status, resident_name, pending_action, owner, status, notes) VALUES ('209B', 'Occupied', 'Martha Lane', 'No action', 'Nursing', 'Closed', 'Stable census bed; no turnover work needed today.')"
        })) return false;
    }
    if (tableIsEmpty("pharmacy_items")) {
        if (!executeAll({
            "INSERT INTO pharmacy_items (review_date, resident_name, item_name, owner, priority, status, notes) VALUES ('2026-04-20', 'Martha Lane', 'Missing evening antibiotic delivery', 'Pharmacy Nurse', 'High', 'Open', 'Confirm courier delivery window and backup dose availability before med pass.')",
            "INSERT INTO pharmacy_items (review_date, resident_name, item_name, owner, priority, status, notes) VALUES ('2026-04-20', 'James Hill', 'Prior authorization follow-up for inhaler refill', 'Business Office', 'Medium', 'In Progress', 'Need payer update and provider office callback before refill delay becomes a risk.')",
            "INSERT INTO pharmacy_items (review_date, resident_name, item_name, owner, priority, status, notes) VALUES ('2026-04-19', 'Evelyn Cross', 'Controlled-drug count discrepancy review', 'DON', 'High', 'Watch', 'Recount complete; documentation review and witness signoff still pending.')"
        })) return false;
    }

    if (tableIsEmpty("transport_items")) {
        if (!executeAll({
            "INSERT INTO transport_items (appointment_date, resident_name, appointment_type, destination, transport_mode, owner, status, notes) VALUES ('2026-04-21', 'James Hill', 'Orthopedic follow-up', 'Regional Ortho Clinic', 'Facility Van', 'Transportation', 'Scheduled', 'Packet and MAR copy needed before departure.')",
            "INSERT INTO transport_items (appointment_date, resident_name, appointment_type, destination, transport_mode, owner, status, notes) VALUES ('2026-04-21', 'Evelyn Cross', 'Dialysis', 'West Dialysis Center', 'Contract Transport', 'Nursing', 'Confirmed', 'Early meal tray and return-time handoff requested.')",
            "INSERT INTO transport_items (appointment_date, resident_name, appointment_type, destination, transport_mode, owner, status, notes) VALUES ('2026-04-22', 'Martha Lane', 'Dental consult', 'Smile Family Dental', 'Family', 'Social Services', 'Needs Packet', 'Consent copy and insurance card still need to be sent with resident.')"
        })) return false;
    }
    return true;
}

QList<QMap<QString, QString>> DatabaseManager::fetchTable(const QString& tableName, const QStringList& columns) const {
    QList<QMap<QString, QString>> rows;
    QSqlQuery q(m_db);
    q.exec(QString("SELECT %1 FROM %2 ORDER BY id DESC").arg(columns.join(", "), tableName));
    while (q.next()) {
        QMap<QString, QString> row;
        for (int i = 0; i < columns.size(); ++i) row[columns[i]] = q.value(i).toString();
        rows.append(row);
    }
    return rows;
}

bool DatabaseManager::addRecord(const QString& tableName, const QMap<QString, QString>& values) {
    QStringList cols = values.keys();
    QStringList binds;
    for (const QString& c : cols) binds << ":" + c;
    QSqlQuery q(m_db);
    q.prepare(QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName, cols.join(", "), binds.join(", ")));
    for (const QString& c : cols) q.bindValue(":" + c, values.value(c));
    return q.exec();
}



bool DatabaseManager::updateRecordById(const QString& tableName, int id, const QMap<QString, QString>& values) {
    QStringList cols = values.keys();
    QStringList assignments;
    for (const QString& c : cols) assignments << (c + " = :" + c);
    QSqlQuery q(m_db);
    q.prepare(QString("UPDATE %1 SET %2 WHERE id = :id").arg(tableName, assignments.join(", ")));
    for (const QString& c : cols) q.bindValue(":" + c, values.value(c));
    q.bindValue(":id", id);
    return q.exec();
}

bool DatabaseManager::admitResident(const QString& residentName, const QString& room, const QString& payer, int admissionId) {
    if (residentName.trimmed().isEmpty()) return false;
    if (!addRecord("residents", {{"resident_name", residentName}, {"room", room}, {"payer", payer}, {"status", "Current"}})) {
        return false;
    }
    if (admissionId >= 0) {
        if (!updateRecordById("admissions", admissionId, {{"status", "Admitted"}})) {
            return false;
        }
    }
    return true;
}

bool DatabaseManager::dischargeResident(int residentId, const QString& residentName) {
    if (residentId < 0) return false;
    if (!updateRecordById("residents", residentId, {{"status", "Discharged"}})) {
        return false;
    }
    if (!residentName.trimmed().isEmpty()) {
        addRecord("admissions", {{"resident_name", residentName}, {"referral_source", "Internal discharge"}, {"planned_date", QDate::currentDate().toString("yyyy-MM-dd")}, {"status", "Discharged"}});
    }
    return true;
}

bool DatabaseManager::addStaffingAssignment(const QString& workDate, const QString& department, const QString& shiftName,
                                           const QString& roleName, const QString& employeeName, const QString& status) {
    return addRecord("staffing_assignments", {{"work_date", workDate}, {"department", department}, {"shift_name", shiftName},
                                             {"role_name", roleName}, {"employee_name", employeeName}, {"status", status}});
}

bool DatabaseManager::updateStaffingAssignmentStatus(int assignmentId, const QString& status) {
    if (assignmentId < 0) return false;
    return updateRecordById("staffing_assignments", assignmentId, {{"status", status}});
}

int DatabaseManager::countMinimumStaffingGaps() const {
    return staffingMinimumSummary().size();
}

QList<QMap<QString, QString>> DatabaseManager::staffingHoursSummary() const {
    QList<QMap<QString, QString>> rows;
    QSqlQuery q(m_db);
    q.exec(
        "SELECT department, shift_name, role_name, "
        "COALESCE(SUM(CASE WHEN status IN ('Filled','Scheduled') THEN 1 ELSE 0 END), 0) AS covered_count, "
        "COALESCE(SUM(CASE WHEN status = 'Open' THEN 1 ELSE 0 END), 0) AS open_count "
        "FROM staffing_assignments "
        "GROUP BY department, shift_name, role_name "
        "ORDER BY department, shift_name, role_name");
    while (q.next()) {
        const int covered = q.value(3).toInt();
        const int open = q.value(4).toInt();
        QMap<QString, QString> row;
        row["department"] = q.value(0).toString();
        row["shift_name"] = q.value(1).toString();
        row["role_name"] = q.value(2).toString();
        row["covered_count"] = QString::number(covered);
        row["open_count"] = QString::number(open);
        row["estimated_hours"] = QString::number(covered * 8);
        row["open_hours"] = QString::number(open * 8);
        rows.append(row);
    }
    return rows;
}

double DatabaseManager::estimatedNursingHprd() const {
    const int currentResidents = countWhere("residents", "status='Current'");
    if (currentResidents <= 0) return 0.0;

    QSqlQuery q(m_db);
    q.exec(
        "SELECT COALESCE(SUM(CASE WHEN status IN ('Filled','Scheduled') THEN 1 ELSE 0 END), 0) "
        "FROM staffing_assignments WHERE department='Nursing'");
    const int nursingCoveredAssignments = q.next() ? q.value(0).toInt() : 0;
    return (nursingCoveredAssignments * 8.0) / static_cast<double>(currentResidents);
}

int DatabaseManager::estimatedMinimumHoursGap() const {
    int totalGapHours = 0;
    const auto rows = staffingMinimumSummary();
    for (const auto& row : rows) {
        totalGapHours += row.value("gap_count").toInt() * 8;
    }
    return totalGapHours;
}

QList<QMap<QString, QString>> DatabaseManager::nursingHprdSummary() const {
    QList<QMap<QString, QString>> rows;
    const int currentResidents = countWhere("residents", "status='Current'");
    if (currentResidents <= 0) return rows;

    QSqlQuery q(m_db);
    q.exec(
        "SELECT role_name, COALESCE(SUM(CASE WHEN status IN ('Filled','Scheduled') THEN 1 ELSE 0 END), 0) AS covered_count "
        "FROM staffing_assignments WHERE department='Nursing' "
        "GROUP BY role_name ORDER BY role_name");
    while (q.next()) {
        const int covered = q.value(1).toInt();
        const double hours = covered * 8.0;
        QMap<QString, QString> row;
        row["role_name"] = q.value(0).toString();
        row["covered_count"] = QString::number(covered);
        row["estimated_hours"] = QString::number(hours, 'f', 1);
        row["hprd"] = QString::number(hours / static_cast<double>(currentResidents), 'f', 2);
        rows.append(row);
    }
    return rows;
}

QList<QMap<QString, QString>> DatabaseManager::staffingMinimumSummary() const {
    QList<QMap<QString, QString>> rows;
    QSqlQuery q(m_db);
    q.exec(
        "SELECT sm.department, sm.shift_name, sm.role_name, sm.minimum_required, "
        "COALESCE(SUM(CASE WHEN sa.status IN ('Filled','Scheduled') THEN 1 ELSE 0 END), 0) AS scheduled_count "
        "FROM staffing_minimums sm "
        "LEFT JOIN staffing_assignments sa "
        "ON sa.department = sm.department AND sa.shift_name = sm.shift_name AND sa.role_name = sm.role_name "
        "GROUP BY sm.department, sm.shift_name, sm.role_name, sm.minimum_required "
        "HAVING scheduled_count < sm.minimum_required "
        "ORDER BY sm.department, sm.shift_name, sm.role_name");
    while (q.next()) {
        QMap<QString, QString> row;
        row["department"] = q.value(0).toString();
        row["shift_name"] = q.value(1).toString();
        row["role_name"] = q.value(2).toString();
        row["minimum_required"] = q.value(3).toString();
        row["scheduled_count"] = q.value(4).toString();
        row["gap_count"] = QString::number(q.value(3).toInt() - q.value(4).toInt());
        row["status"] = "Below Minimum";
        rows.append(row);
    }
    return rows;
}

int DatabaseManager::countWhere(const QString& tableName, const QString& whereClause) const {
    QSqlQuery q(m_db);
    QString sql = QString("SELECT COUNT(*) FROM %1").arg(tableName);
    if (!whereClause.trimmed().isEmpty()) sql += " WHERE " + whereClause;
    q.exec(sql);
    return q.next() ? q.value(0).toInt() : 0;
}

QList<QPair<QString, QString>> DatabaseManager::actionCenterItems() const {
    return {
        {"Admissions", "Move accepted referrals into current census from the Admissions page."},
        {"Discharges", "Mark completed discharges from the Residents page to keep census accurate."},
        {"Staffing", "Use the Staffing page to add assignments and flip open shifts to filled coverage."},
        {"Minimum staffing", QString("%1 assignment group(s) are still below minimum coverage.").arg(countMinimumStaffingGaps())},
        {"Hours gap", QString("Estimated uncovered hours against minimum staffing: %1.").arg(estimatedMinimumHoursGap())},
        {"Survey readiness", "Abuse policy interview prep still open."},
        {"Managed care", "Authorization extension remains at risk."},
        {"Risk management", "Family grievance follow-up call overdue."},
        {"Bed board", QString("%1 room-turnover or bed-board item(s) remain open.").arg(countWhere("bed_board", "status!='Closed'"))},
        {"Transportation", QString("%1 transport or outside-appointment item(s) still need follow-up.").arg(countWhere("transport_items", "status!='Returned' AND status!='Closed'"))},
        {"Pharmacy", QString("%1 pharmacy or medication-system item(s) are open or on watch.").arg(countWhere("pharmacy_items", "status='Open' OR status='Watch' OR status='In Progress'"))}
    };
}
