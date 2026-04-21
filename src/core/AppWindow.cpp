#include "AppWindow.h"
#include "../data/DatabaseManager.h"
#include "../ui/pages/AdmissionsPage.h"
#include "../ui/pages/AlertsPage.h"
#include "../ui/pages/BedBoardPage.h"
#include "../ui/pages/BudgetPage.h"
#include "../ui/pages/CompliancePage.h"
#include "../ui/pages/CredentialingPage.h"
#include "../ui/pages/DashboardPage.h"
#include "../ui/pages/DietaryPage.h"
#include "../ui/pages/EnvironmentalRoundsPage.h"
#include "../ui/pages/HuddlePage.h"
#include "../ui/pages/InfectionControlPage.h"
#include "../ui/pages/IncidentsPage.h"
#include "../ui/pages/ManagedCarePage.h"
#include "../ui/pages/PharmacyPage.h"
#include "../ui/pages/PreparednessPage.h"
#include "../ui/pages/QapiPage.h"
#include "../ui/pages/QualityMeasuresPage.h"
#include "../ui/pages/ReportsPage.h"
#include "../ui/pages/ResidentsPage.h"
#include "../ui/pages/RiskManagementPage.h"
#include "../ui/pages/StaffingPage.h"
#include "../ui/pages/SurveyReadinessPage.h"
#include "../ui/pages/TasksPage.h"
#include "../ui/pages/TransportationPage.h"
#include "../ui/pages/WorkflowCenterPage.h"
#include "../ui/pages/DocumentCenterPage.h"
#include "../ui/pages/CensusManagementPage.h"
#include "../ui/pages/MdsTripleCheckPage.h"
#include "../ui/pages/SurveyCommandCenterPage.h"
#include "../ui/pages/OutbreakCommandPage.h"

#include <QAbstractScrollArea>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>

AppWindow::AppWindow(DatabaseManager* db, const QString& fullName, const QString& roleName, QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("LTC Administrator Operations Dashboard v33 Referral + Document Intake");
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
    headerLayout->setSpacing(8);

    auto* topRow = new QHBoxLayout();
    auto* title = new QLabel("LTC Administrator Operations Dashboard", header);
    title->setObjectName("appTitle");
    auto* userBadge = new QLabel(QString("Signed in: %1 · %2").arg(fullName, roleName), header);
    userBadge->setStyleSheet("background:#eef4f8; border:1px solid #d9e2ec; border-radius:12px; padding:8px 12px; color:#334e68; font-weight:600;");
    topRow->addWidget(title, 1);
    topRow->addWidget(userBadge, 0, Qt::AlignRight);

    auto* subtitle = new QLabel(
        "Enhanced v33 workspace with role-aware sign-in, actual document intake, a richer referral waitlist, admit-from-waitlist support, diagnosis carry-forward, MDS / ARD / triple-check intake, survey command tools, outbreak tracking, workflow cleanup, and due-soon alerts.",
        header);
    subtitle->setObjectName("appSubtitle");
    subtitle->setWordWrap(true);

    headerLayout->addLayout(topRow);
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
    auto* navHint = new QLabel(QString("%1 access profile. Pages scroll automatically on smaller screens.").arg(roleName), sidebar);
    navHint->setObjectName("sidebarHint");
    navHint->setWordWrap(true);

    auto* nav = new QListWidget(sidebar);
    nav->setObjectName("sideNav");
    nav->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    nav->setSpacing(4);

    const QList<QPair<QString,QWidget*>> pages = {
        {"Dashboard", new DashboardPage(db)},
        {"Alerts", new AlertsPage(db)},
        {"Residents", new ResidentsPage(db)},
        {"Admissions", new AdmissionsPage(db)},
        {"Staffing", new StaffingPage(db)},
        {"Tasks", new TasksPage(db)},
        {"QAPI / PIP", new QapiPage(db)},
        {"Budget / Labor", new BudgetPage(db)},
        {"Compliance", new CompliancePage(db)},
        {"Huddle", new HuddlePage(db)},
        {"Incidents", new IncidentsPage(db)},
        {"Survey Readiness", new SurveyReadinessPage(db)},
        {"Quality Measures", new QualityMeasuresPage(db)},
        {"Managed Care", new ManagedCarePage(db)},
        {"Credentialing", new CredentialingPage(db)},
        {"Preparedness", new PreparednessPage(db)},
        {"Infection Control", new InfectionControlPage(db)},
        {"Grievances", new RiskManagementPage(db)},
        {"Environmental Rounds", new EnvironmentalRoundsPage(db)},
        {"Bed Board", new BedBoardPage(db)},
        {"Transportation", new TransportationPage(db)},
        {"Pharmacy / Meds", new PharmacyPage(db)},
        {"Dietary / Nutrition", new DietaryPage(db)},
        {"Document Center", new DocumentCenterPage(db)},
        {"Census Management", new CensusManagementPage(db)},
        {"MDS / Triple Check", new MdsTripleCheckPage(db)},
        {"Survey Command Center", new SurveyCommandCenterPage(db)},
        {"Outbreak Command", new OutbreakCommandPage(db)},
        {"Reports & Export", new ReportsPage(db)},
        {"Workflow Center", new WorkflowCenterPage(db)}
    };

    QStringList allowed;
    if (roleName == "Administrator") {
        for (const auto& pair : pages) allowed << pair.first;
    } else if (roleName == "Director of Nursing") {
        allowed = {"Dashboard","Alerts","Residents","Admissions","Staffing","Tasks","QAPI / PIP","Huddle","Incidents","Survey Readiness","Quality Measures","Credentialing","Preparedness","Infection Control","Grievances","Pharmacy / Meds","Dietary / Nutrition","Document Center","Census Management","MDS / Triple Check","Survey Command Center","Outbreak Command","Reports & Export","Workflow Center"};
    } else if (roleName == "Admissions Director") {
        allowed = {"Dashboard","Alerts","Residents","Admissions","Tasks","Managed Care","Bed Board","Transportation","Document Center","Census Management","MDS / Triple Check","Survey Command Center","Outbreak Command","Reports & Export","Workflow Center"};
    } else if (roleName == "Staffing Coordinator") {
        allowed = {"Dashboard","Alerts","Staffing","Tasks","Credentialing","Preparedness","Document Center","Reports & Export","Workflow Center"};
    } else {
        allowed = {"Dashboard","Alerts","Residents","Admissions","Staffing","Quality Measures","Document Center","Census Management","MDS / Triple Check","Survey Command Center","Outbreak Command","Reports & Export","Workflow Center"};
    }

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

    for (const auto& pair : pages) {
        if (!allowed.contains(pair.first)) continue;
        nav->addItem(pair.first);
        stack->addWidget(wrapPage(pair.second));
    }

    QObject::connect(nav, &QListWidget::currentRowChanged, stack, &QStackedWidget::setCurrentIndex);
    nav->setCurrentRow(0);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>{260, 1120});

    shellLayout->addWidget(splitter, 1);
    setCentralWidget(shell);
}
