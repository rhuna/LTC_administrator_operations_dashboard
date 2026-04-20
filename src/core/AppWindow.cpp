#include "AppWindow.h"

#include <QAction>
#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "data/DatabaseManager.h"
#include "ui/pages/AdmissionsPage.h"
#include "ui/pages/BudgetPage.h"
#include "ui/pages/CompliancePage.h"
#include "ui/pages/DashboardPage.h"
#include "ui/pages/HuddlePage.h"
#include "ui/pages/IncidentsPage.h"
#include "ui/pages/QualityMeasuresPage.h"
#include "ui/pages/TrainingPage.h"
#include "ui/pages/EmergencyPreparednessPage.h"
#include "ui/pages/InfectionControlPage.h"
#include "ui/pages/QapiPage.h"
#include "ui/pages/ResidentsPage.h"
#include "ui/pages/StaffingPage.h"
#include "ui/pages/SurveyReadinessPage.h"
#include "ui/pages/TasksPage.h"

AppWindow::AppWindow(DatabaseManager* db, QWidget* parent)
    : QMainWindow(parent),
      db_(db),
      stackedWidget_(new QStackedWidget(this)),
      navGroup_(new QButtonGroup(this)),
      dashboardPage_(new DashboardPage(db, this)),
      residentsPage_(new ResidentsPage(db, this)),
      admissionsPage_(new AdmissionsPage(db, this)),
      staffingPage_(new StaffingPage(db, this)),
      tasksPage_(new TasksPage(db, this)),
      qapiPage_(new QapiPage(db, this)),
      budgetPage_(new BudgetPage(db, this)),
      compliancePage_(new CompliancePage(db, this)),
      huddlePage_(new HuddlePage(db, this)),
      qualityMeasuresPage_(new QualityMeasuresPage(db, this)),
      trainingPage_(new TrainingPage(db, this)),
      emergencyPreparednessPage_(new EmergencyPreparednessPage(db, this)),
      infectionControlPage_(new InfectionControlPage(db, this)),
      incidentsPage_(new IncidentsPage(db, this)),
      surveyPage_(new SurveyReadinessPage(db, this)) {
    setWindowTitle("LTC Administrator Operations Dashboard v12");

    auto* central = new QWidget(this);
    auto* root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto* navPanel = new QFrame(central);
    navPanel->setObjectName("NavPanel");
    navPanel->setFixedWidth(280);
    auto* navLayout = new QVBoxLayout(navPanel);
    navLayout->setContentsMargins(16, 20, 16, 20);
    navLayout->setSpacing(10);

    auto* title = new QLabel("LTC Ops Dashboard", navPanel);
    title->setObjectName("AppTitle");
    navLayout->addWidget(title);

    auto* subtitle = new QLabel("Administrator desktop v12", navPanel);
    subtitle->setObjectName("NavSubtitle");
    navLayout->addWidget(subtitle);
    navLayout->addSpacing(12);

    const QStringList navNames = {
        "Dashboard",
        "Residents",
        "Admissions",
        "Staffing",
        "Tasks",
        "QAPI / PIP",
        "Budget / Labor",
        "Compliance Calendar",
        "Daily Ops Huddle",
        "Quality Measures",
        "Training / Credentialing",
        "Emergency Preparedness",
        "Infection Control",
        "Incidents",
        "Survey Readiness"
    };

    for (int i = 0; i < navNames.size(); ++i) {
        auto* button = new QPushButton(navNames[i], navPanel);
        button->setObjectName("NavButton");
        button->setCheckable(true);
        navGroup_->addButton(button, i);
        navLayout->addWidget(button);
    }
    navLayout->addStretch();

    stackedWidget_->addWidget(dashboardPage_);
    stackedWidget_->addWidget(residentsPage_);
    stackedWidget_->addWidget(admissionsPage_);
    stackedWidget_->addWidget(staffingPage_);
    stackedWidget_->addWidget(tasksPage_);
    stackedWidget_->addWidget(qapiPage_);
    stackedWidget_->addWidget(budgetPage_);
    stackedWidget_->addWidget(compliancePage_);
    stackedWidget_->addWidget(huddlePage_);
    stackedWidget_->addWidget(qualityMeasuresPage_);
    stackedWidget_->addWidget(trainingPage_);
    stackedWidget_->addWidget(emergencyPreparednessPage_);
    stackedWidget_->addWidget(infectionControlPage_);
    stackedWidget_->addWidget(incidentsPage_);
    stackedWidget_->addWidget(surveyPage_);

    root->addWidget(navPanel);
    root->addWidget(stackedWidget_, 1);
    setCentralWidget(central);

    connect(navGroup_, &QButtonGroup::idClicked, this, &AppWindow::showPage);
    if (auto* firstButton = navGroup_->button(0)) {
        firstButton->setChecked(true);
    }
    showPage(0);

    auto* refreshAction = new QAction("Refresh", this);
    connect(refreshAction, &QAction::triggered, this, &AppWindow::refreshAllPages);
    menuBar()->addAction(refreshAction);
}

void AppWindow::showPage(int index) {
    stackedWidget_->setCurrentIndex(index);
}

void AppWindow::refreshAllPages() {
    dashboardPage_->refresh();
    residentsPage_->refresh();
    admissionsPage_->refresh();
    staffingPage_->refresh();
    tasksPage_->refresh();
    qapiPage_->refresh();
    budgetPage_->refresh();
    compliancePage_->refresh();
    huddlePage_->refresh();
    qualityMeasuresPage_->refresh();
    trainingPage_->refresh();
    emergencyPreparednessPage_->refresh();
    infectionControlPage_->refresh();
    incidentsPage_->refresh();
    surveyPage_->refresh();
}
