#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QString>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager() override;

    bool initialize();
    QString databasePath() const;

    QSqlDatabase database() const;

    int currentCensus() const;
    int bedCapacity() const;
    int openIncidents() const;
    int openTasks() const;
    double averageStaffHoursPerResidentDay() const;
    int surveyItemsAtRisk() const;

    QSqlQueryModel* createResidentsModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createStaffingModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createTasksModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createIncidentsModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createSurveyModel(QObject* parent = nullptr) const;

    bool addTask(const QString& title, const QString& owner, const QString& priority, const QString& dueDate);
    bool addIncident(const QString& eventDate, const QString& residentName, const QString& severity, const QString& description);

private:
    QString connectionName_;
    QSqlDatabase database_;

    bool openDatabase();
    bool createTables();
    bool seedData();
    bool executeStatement(const QString& sql) const;
};
