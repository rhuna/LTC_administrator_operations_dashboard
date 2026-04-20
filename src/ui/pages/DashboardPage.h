#pragma once

#include <QWidget>

class DatabaseManager;
class KpiCard;
class QListWidget;

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private:
    DatabaseManager* db_;
    KpiCard* censusCard_;
    KpiCard* occupancyCard_;
    KpiCard* incidentsCard_;
    KpiCard* tasksCard_;
    KpiCard* hprdCard_;
    KpiCard* surveyRiskCard_;
    QListWidget* priorityList_;
};
