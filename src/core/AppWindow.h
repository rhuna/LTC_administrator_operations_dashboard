#pragma once

#include <QMainWindow>

class DatabaseManager;
class DashboardPage;
class AdmissionsPage;
class IncidentsPage;
class ResidentsPage;
class StaffingPage;
class SurveyReadinessPage;
class TasksPage;
class QapiPage;
class BudgetPage;
class CompliancePage;
class HuddlePage;
class QualityMeasuresPage;
class TrainingPage;
class EmergencyPreparednessPage;
class InfectionControlPage;
class QButtonGroup;
class QStackedWidget;

class AppWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit AppWindow(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void showPage(int index);
    void refreshAllPages();

private:
    DatabaseManager* db_;
    QStackedWidget* stackedWidget_;
    QButtonGroup* navGroup_;
    DashboardPage* dashboardPage_;
    ResidentsPage* residentsPage_;
    AdmissionsPage* admissionsPage_;
    StaffingPage* staffingPage_;
    TasksPage* tasksPage_;
    QapiPage* qapiPage_;
    BudgetPage* budgetPage_;
    CompliancePage* compliancePage_;
    HuddlePage* huddlePage_;
    QualityMeasuresPage* qualityMeasuresPage_;
    TrainingPage* trainingPage_;
    EmergencyPreparednessPage* emergencyPreparednessPage_;
    InfectionControlPage* infectionControlPage_;
    IncidentsPage* incidentsPage_;
    SurveyReadinessPage* surveyPage_;
};
