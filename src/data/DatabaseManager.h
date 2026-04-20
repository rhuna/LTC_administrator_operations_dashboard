#pragma once
#include <QList>
#include <QMap>
#include <QPair>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool initialize();
    QList<QMap<QString, QString>> fetchTable(const QString& tableName, const QStringList& columns) const;
    bool addRecord(const QString& tableName, const QMap<QString, QString>& values);
    bool updateRecordById(const QString& tableName, int id, const QMap<QString, QString>& values);
    bool admitResident(const QString& residentName, const QString& room, const QString& payer, int admissionId = -1);
    bool dischargeResident(int residentId, const QString& residentName);
    bool addStaffingAssignment(const QString& workDate, const QString& department, const QString& shiftName,
                              const QString& roleName, const QString& employeeName, const QString& status);
    bool updateStaffingAssignmentStatus(int assignmentId, const QString& status);
    int countWhere(const QString& tableName, const QString& whereClause = QString()) const;
    int countMinimumStaffingGaps() const;
    QList<QMap<QString, QString>> staffingMinimumSummary() const;
    QList<QMap<QString, QString>> staffingHoursSummary() const;
    QList<QMap<QString, QString>> nursingHprdSummary() const;
    double estimatedNursingHprd() const;
    int estimatedMinimumHoursGap() const;
    QList<QPair<QString, QString>> actionCenterItems() const;

private:
    QSqlDatabase m_db;
    bool createTables();
    bool seedData();
    bool executeAll(const QStringList& statements) const;
    bool tableIsEmpty(const QString& tableName) const;
};
