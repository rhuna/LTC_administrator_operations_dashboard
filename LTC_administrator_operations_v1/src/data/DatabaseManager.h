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
    bool authenticateUser(const QString& username, const QString& password, QString* fullName, QString* role) const;
    QList<QMap<QString, QString>> fetchUsers() const;
    bool addRecord(const QString& tableName, const QMap<QString, QString>& values);
    bool updateRecordById(const QString& tableName, int id, const QMap<QString, QString>& values);
    bool deleteRecordById(const QString& tableName, int id);
    bool archiveRecordById(const QString& tableName, int id);
    bool admitResident(const QString& residentName, const QString& room, const QString& payer, int admissionId = -1, const QString& diagnosisSummary = QString());
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
    int overdueAlertCount() const;
    int dueSoonAlertCount() const;
    QList<QMap<QString, QString>> alertItems() const;
    QList<QPair<QString, QString>> actionCenterItems() const;
    QString databaseFilePath() const;
    QString backupDirectoryPath() const;
    QStringList availableBackupFiles() const;
    bool createBackupCopy(QString* createdPath = nullptr);
    bool restoreFromBackup(const QString& backupPath);
    bool exportJsonSnapshot(QString* exportPath = nullptr) const;
    QList<QMap<QString, QString>> auditLogItems() const;
    QList<QMap<QString, QString>> validationAlerts() const;
    QString lastErrorText() const;

private:
    QSqlDatabase m_db;
    QString m_lastError;
    bool createTables();
    bool seedData();
    bool executeAll(const QStringList& statements) const;
    bool tableIsEmpty(const QString& tableName) const;
    bool logAuditEvent(const QString& moduleName, const QString& actionName, const QString& itemName, const QString& actorName, const QString& details);
};
