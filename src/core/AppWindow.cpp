#include "AppWindow.h"
#include "../data/DatabaseManager.h"
#include "../ui/pages/AdmissionsPage.h"
#include "../ui/pages/BudgetPage.h"
#include "../ui/pages/CompliancePage.h"
#include "../ui/pages/CredentialingPage.h"
#include "../ui/pages/DashboardPage.h"
#include "../ui/pages/EnvironmentalRoundsPage.h"
#include "../ui/pages/HuddlePage.h"
#include "../ui/pages/InfectionControlPage.h"
#include "../ui/pages/IncidentsPage.h"
#include "../ui/pages/ManagedCarePage.h"
#include "../ui/pages/PreparednessPage.h"
#include "../ui/pages/QapiPage.h"
#include "../ui/pages/QualityMeasuresPage.h"
#include "../ui/pages/ResidentsPage.h"
#include "../ui/pages/RiskManagementPage.h"
#include "../ui/pages/StaffingPage.h"
#include "../ui/pages/SurveyReadinessPage.h"
#include "../ui/pages/TasksPage.h"

#include <QAbstractScrollArea>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>

AppWindow::AppWindow(DatabaseManager* db, QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("LTC Administrator Operations Dashboard v18");
    resize(1440, 920);
    setMinimumSize(1180, 760);

    auto* shell = new QWidget(this);
    auto* shellLayout = new QVBoxLayout(shell);
    shellLayout->setContentsMargins(20, 18, 20, 18);
    shellLayout->setSpacing(14);

    auto* header = new QFrame(shell);
    header->setObjectName("appHeader");
    auto* headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(20, 18, 20, 18);
    headerLayout->setSpacing(2);

    auto* title = new QLabel("LTC Administrator Operations Dashboard", header);
    title->setObjectName("appTitle");
    auto* subtitle = new QLabel(
        "Cleaner v18 workspace with staffing actions, minimum staffing visibility, lighter navigation, and easier scanning.",
        header);
    subtitle->setObjectName("appSubtitle");
    subtitle->setWordWrap(true);
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    shellLayout->addWidget(header);

    auto* splitter = new QSplitter(shell);
    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(1);

    auto* sidebar = new QFrame(splitter);
    sidebar->setObjectName("sidebarCard");
    auto* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(12, 14, 12, 14);
    sidebarLayout->setSpacing(10);

    auto* navLabel = new QLabel("Modules", sidebar);
    navLabel->setObjectName("sidebarHeading");
    auto* navHint = new QLabel("Select a workspace. Pages scroll automatically on smaller screens.", sidebar);
    navHint->setObjectName("sidebarHint");
    navHint->setWordWrap(true);

    auto* nav = new QListWidget(sidebar);
    nav->setObjectName("sideNav");
    nav->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    nav->setSpacing(4);

    const QStringList items = {
        "Dashboard", "Residents", "Admissions", "Staffing", "Tasks", "QAPI / PIP", "Budget / Labor",
        "Compliance", "Huddle", "Incidents", "Survey Readiness", "Quality Measures",
        "Managed Care", "Credentialing", "Preparedness", "Infection Control",
        "Grievances", "Environmental Rounds"
    };
    nav->addItems(items);

    sidebarLayout->addWidget(navLabel);
    sidebarLayout->addWidget(navHint);
    sidebarLayout->addWidget(nav, 1);

    auto* stack = new QStackedWidget(splitter);
    stack->setObjectName("contentStack");

    auto wrapPage = [&](QWidget* page) -> QWidget* {
        auto* scroll = new QScrollArea(splitter);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll->setWidget(page);
        return scroll;
    };

    stack->addWidget(wrapPage(new DashboardPage(db)));
    stack->addWidget(wrapPage(new ResidentsPage(db)));
    stack->addWidget(wrapPage(new AdmissionsPage(db)));
    stack->addWidget(wrapPage(new StaffingPage(db)));
    stack->addWidget(wrapPage(new TasksPage(db)));
    stack->addWidget(wrapPage(new QapiPage(db)));
    stack->addWidget(wrapPage(new BudgetPage(db)));
    stack->addWidget(wrapPage(new CompliancePage(db)));
    stack->addWidget(wrapPage(new HuddlePage(db)));
    stack->addWidget(wrapPage(new IncidentsPage(db)));
    stack->addWidget(wrapPage(new SurveyReadinessPage(db)));
    stack->addWidget(wrapPage(new QualityMeasuresPage(db)));
    stack->addWidget(wrapPage(new ManagedCarePage(db)));
    stack->addWidget(wrapPage(new CredentialingPage(db)));
    stack->addWidget(wrapPage(new PreparednessPage(db)));
    stack->addWidget(wrapPage(new InfectionControlPage(db)));
    stack->addWidget(wrapPage(new RiskManagementPage(db)));
    stack->addWidget(wrapPage(new EnvironmentalRoundsPage(db)));

    QObject::connect(nav, &QListWidget::currentRowChanged, stack, &QStackedWidget::setCurrentIndex);
    nav->setCurrentRow(0);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>{240, 1140});

    shellLayout->addWidget(splitter, 1);
    setCentralWidget(shell);
}
