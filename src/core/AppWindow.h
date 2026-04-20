#pragma once

#include <QMainWindow>

class DatabaseManager;
class DashboardPage;
class IncidentsPage;
class ResidentsPage;
class StaffingPage;
class SurveyReadinessPage;
class TasksPage;
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
    StaffingPage* staffingPage_;
    TasksPage* tasksPage_;
    IncidentsPage* incidentsPage_;
    SurveyReadinessPage* surveyPage_;
};
