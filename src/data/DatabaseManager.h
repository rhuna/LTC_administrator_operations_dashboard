#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QString>
#include <QStringList>

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
    int pendingAdmissions() const;
    int plannedDischarges() const;
    int highPriorityTasks() const;
    int agencyShifts() const;
    int activeQapiProjects() const;
    double totalOvertimeHours() const;
    double monthlyLaborBudgetVariance() const;
    int budgetItemsAtRisk() const;
    int complianceItemsDueSoon() const;
    int pendingStaffingChanges() const;
    int openHuddleItems() const;
    int qualityMeasuresOffTarget() const;
    int trainingItemsDueSoon() const;
    int emergencyPreparednessItemsDueSoon() const;
    int infectionControlItemsOpen() const;
    QString facilityName() const;
    QStringList dashboardFocusItems() const;

    QSqlQueryModel* createResidentsModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createStaffingModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createTasksModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createIncidentsModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createSurveyModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createAdmissionsModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createQapiModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createBudgetModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createComplianceModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createStaffingChangesModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createHuddleItemsModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createQualityMeasuresModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createTrainingModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createEmergencyPreparednessModel(QObject* parent = nullptr) const;
    QSqlQueryModel* createInfectionControlModel(QObject* parent = nullptr) const;

    bool addTask(const QString& title, const QString& owner, const QString& priority, const QString& dueDate);
    bool addIncident(const QString& eventDate, const QString& residentName, const QString& severity, const QString& description);
    bool addAdmission(const QString& residentName,
                      const QString& referralSource,
                      const QString& payerSource,
                      const QString& expectedDate,
                      const QString& status);
    bool addQapiProject(const QString& topic,
                        const QString& owner,
                        const QString& source,
                        const QString& stage,
                        const QString& nextReview,
                        const QString& aimStatement);
    bool addBudgetItem(const QString& category,
                       const QString& owner,
                       double monthlyBudget,
                       double actualSpend,
                       const QString& status,
                       const QString& nextReview);
    bool addComplianceItem(const QString& area,
                           const QString& itemName,
                           const QString& owner,
                           const QString& dueDate,
                           const QString& status);
    bool addStaffingChange(const QString& changeDate,
                           const QString& department,
                           const QString& shiftName,
                           const QString& changeType,
                           const QString& positionName,
                           const QString& employeeName,
                           const QString& impactLevel,
                           const QString& status,
                           const QString& notes);
    bool addHuddleItem(const QString& huddleDate,
                       const QString& shiftName,
                       const QString& department,
                       const QString& topic,
                       const QString& owner,
                       const QString& priority,
                       const QString& status,
                       const QString& notes);
    bool addQualityMeasure(const QString& measureName,
                           const QString& measureSet,
                           const QString& owner,
                           double currentValue,
                           double targetValue,
                           const QString& nextReview,
                           const QString& status,
                           const QString& notes);
    bool addTrainingItem(const QString& trainingArea,
                         const QString& employeeName,
                         const QString& roleName,
                         const QString& dueDate,
                         const QString& status,
                         const QString& notes);
    bool addEmergencyPreparednessItem(const QString& drillDate,
                                      const QString& scenarioName,
                                      const QString& owner,
                                      const QString& nextDrillDate,
                                      const QString& status,
                                      const QString& notes);
    bool addInfectionControlItem(const QString& eventDate,
                                 const QString& areaName,
                                 const QString& owner,
                                 const QString& nextReview,
                                 const QString& status,
                                 const QString& notes);

private:
    QString connectionName_;
    QSqlDatabase database_;

    bool openDatabase();
    bool createTables();
    bool seedData();
    bool executeStatement(const QString& sql) const;
};
