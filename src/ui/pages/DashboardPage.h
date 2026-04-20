#pragma once

#include <QWidget>

class DatabaseManager;
class KpiCard;
class QListWidget;
class QLabel;

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private:
    DatabaseManager* db_;
    QLabel* subtitleLabel_;
    KpiCard* censusCard_;
    KpiCard* occupancyCard_;
    KpiCard* incidentsCard_;
    KpiCard* tasksCard_;
    KpiCard* hprdCard_;
    KpiCard* surveyRiskCard_;
    KpiCard* admissionsCard_;
    KpiCard* dischargeCard_;
    KpiCard* agencyCard_;
    KpiCard* qapiCard_;
    KpiCard* overtimeCard_;
    KpiCard* varianceCard_;
    KpiCard* complianceCard_;
    KpiCard* staffingChangeCard_;
    KpiCard* huddleCard_;
    KpiCard* qualityCard_;
    KpiCard* trainingCard_;
    KpiCard* emergencyCard_;
    KpiCard* infectionCard_;
    QListWidget* priorityList_;
};
