#include "DatabaseManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QUuid>
#include <QtMath>

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
               "bed_capacity INTEGER NOT NULL)" ) &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS residents ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name TEXT NOT NULL,"
               "payer_source TEXT NOT NULL,"
               "room TEXT NOT NULL,"
               "acuity TEXT NOT NULL,"
               "admit_date TEXT NOT NULL,"
               "status TEXT NOT NULL)" ) &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS staffing ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "department TEXT NOT NULL,"
               "scheduled INTEGER NOT NULL,"
               "worked INTEGER NOT NULL,"
               "agency INTEGER NOT NULL,"
               "overtime_hours REAL NOT NULL,"
               "hprd REAL NOT NULL)" ) &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS tasks ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "title TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "priority TEXT NOT NULL,"
               "due_date TEXT NOT NULL,"
               "status TEXT NOT NULL DEFAULT 'Open')" ) &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS incidents ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "event_date TEXT NOT NULL,"
               "resident_name TEXT NOT NULL,"
               "severity TEXT NOT NULL,"
               "description TEXT NOT NULL,"
               "status TEXT NOT NULL DEFAULT 'Open')" ) &&
           executeStatement(
               "CREATE TABLE IF NOT EXISTS survey_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "domain_name TEXT NOT NULL,"
               "item_name TEXT NOT NULL,"
               "owner TEXT NOT NULL,"
               "readiness_status TEXT NOT NULL,"
               "next_review TEXT NOT NULL)" );
}

bool DatabaseManager::seedData() {
    QSqlQuery checkFacility(database_);
    if (!checkFacility.exec("SELECT COUNT(*) FROM facility_info") || !checkFacility.next()) {
        return false;
    }
    if (checkFacility.value(0).toInt() > 0) {
        return true;
    }

    if (!executeStatement("INSERT INTO facility_info (id, facility_name, bed_capacity) VALUES (1, 'Green Valley Care Center', 120)")) {
        return false;
    }

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

    const QStringList staffing = {
        "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('Nursing', 28, 26, 3, 22.5, 3.85)",
        "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('CNA', 34, 31, 5, 27.0, 2.95)",
        "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('Dietary', 8, 8, 0, 2.0, 0.74)",
        "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('Housekeeping', 7, 6, 1, 4.5, 0.58)",
        "INSERT INTO staffing (department, scheduled, worked, agency, overtime_hours, hprd) VALUES ('Activities', 3, 3, 0, 0.0, 0.21)"
    };

    const QStringList tasks = {
        "INSERT INTO tasks (title, owner, priority, due_date, status) VALUES ('Complete monthly QAPI packet', 'Administrator', 'High', '2026-04-24', 'Open')",
        "INSERT INTO tasks (title, owner, priority, due_date, status) VALUES ('Review infection control audit', 'DON', 'High', '2026-04-22', 'Open')",
        "INSERT INTO tasks (title, owner, priority, due_date, status) VALUES ('Renew dietary sanitation log', 'Dietary Manager', 'Medium', '2026-04-25', 'Open')",
        "INSERT INTO tasks (title, owner, priority, due_date, status) VALUES ('Finalize staffing contingency plan', 'Scheduler', 'Medium', '2026-04-26', 'Open')"
    };

    const QStringList incidents = {
        "INSERT INTO incidents (event_date, resident_name, severity, description, status) VALUES ('2026-04-18', 'Mary Johnson', 'Moderate', 'Unwitnessed fall with no apparent injury.', 'Open')",
        "INSERT INTO incidents (event_date, resident_name, severity, description, status) VALUES ('2026-04-16', 'James Wilson', 'Low', 'Medication variance identified during evening pass.', 'Closed')",
        "INSERT INTO incidents (event_date, resident_name, severity, description, status) VALUES ('2026-04-19', 'Rosa Brown', 'High', 'Elopement risk alarm malfunction found during round.', 'Open')"
    };

    const QStringList survey = {
        "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Clinical', 'Accident / Supervision', 'DON', 'At Risk', '2026-04-21')",
        "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Clinical', 'Medication Storage', 'Unit Manager', 'Ready', '2026-04-28')",
        "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Life Safety', 'Fire Drill Documentation', 'Maintenance Director', 'At Risk', '2026-04-23')",
        "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Dietary', 'Temperature Logs', 'Dietary Manager', 'Ready', '2026-04-27')",
        "INSERT INTO survey_items (domain_name, item_name, owner, readiness_status, next_review) VALUES ('Resident Rights', 'Grievance Tracking', 'Administrator', 'Needs Review', '2026-04-22')"
    };

    for (const auto& sql : residents) {
        if (!executeStatement(sql)) return false;
    }
    for (const auto& sql : staffing) {
        if (!executeStatement(sql)) return false;
    }
    for (const auto& sql : tasks) {
        if (!executeStatement(sql)) return false;
    }
    for (const auto& sql : incidents) {
        if (!executeStatement(sql)) return false;
    }
    for (const auto& sql : survey) {
        if (!executeStatement(sql)) return false;
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
    query.exec("SELECT COUNT(*) FROM survey_items WHERE readiness_status = 'At Risk'");
    return query.next() ? query.value(0).toInt() : 0;
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
    model->setQuery("SELECT title AS Task, owner AS Owner, priority AS Priority, due_date AS 'Due Date', status AS Status FROM tasks ORDER BY due_date", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createIncidentsModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT event_date AS Date, resident_name AS Resident, severity AS Severity, description AS Description, status AS Status FROM incidents ORDER BY event_date DESC", database_);
    return model;
}

QSqlQueryModel* DatabaseManager::createSurveyModel(QObject* parent) const {
    auto* model = new QSqlQueryModel(parent);
    model->setQuery("SELECT domain_name AS Domain, item_name AS Item, owner AS Owner, readiness_status AS Status, next_review AS 'Next Review' FROM survey_items ORDER BY next_review", database_);
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
