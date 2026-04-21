#include "AppWindow.h"
#include "../data/DatabaseManager.h"
#include "../ui/pages/AdmissionsPage.h"
#include "../ui/pages/AlertsPage.h"
#include "../ui/pages/BedBoardPage.h"
#include "../ui/pages/BudgetPage.h"
#include "../ui/pages/CompliancePage.h"
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
#include "../ui/pages/DashboardCustomizePage.h"
#include "../ui/pages/CalendarPage.h"
#include "../ui/pages/MetricsChartsPage.h"
#include "../ui/pages/KpiTrendEnginePage.h"
#include "../ui/pages/ServiceLayerPage.h"
#include "../ui/pages/ExternalSyncReadinessPage.h"
#include "../ui/pages/ReleaseCandidatePage.h"
#include "../ui/pages/SopCenterPage.h"
#include "../ui/pages/CareConferencePage.h"
#include "../ui/pages/TherapyRehabPage.h"
#include "../ui/pages/SocialServicesPage.h"
#include "../ui/pages/DepartmentDashboardsPage.h"
#include "../ui/pages/BackupRestorePage.h"
#include "../ui/pages/AuditLogPage.h"
#include "../ui/pages/FormsValidationPage.h"
#include "../ui/pages/RevenueCyclePage.h"
#include "../ui/pages/ContractManagementPage.h"

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
    setWindowTitle("LTC Administrator Operations Dashboard v59 Simplified Operations Layout");
    resize(1500, 940);
    setMinimumSize(1220, 780);

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
        "v59 removes the extra search, shift handoff, and credentialing tabs, keeps staffing focused on numbers and ratios, folds grievances into Social Services, and expands Environmental Services to cover maintenance, laundry, and housekeeping from one page.",
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

    auto* navLabel = new QLabel("Navigation", sidebar);
    navLabel->setObjectName("sidebarHeading");
    auto* navHint = new QLabel(QString("%1 profile · simplified tab set · pages scroll automatically on smaller screens.").arg(roleName), sidebar);
    navHint->setObjectName("sidebarHint");
    navHint->setWordWrap(true);

    auto* nav = new QListWidget(sidebar);
    nav->setObjectName("sideNav");
    nav->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    nav->setSpacing(4);

    const QList<QPair<QString,QWidget*>> pages = {
        {"Dashboard", new DashboardPage(db)},
        {"Dashboard Setup", new DashboardCustomizePage(db)},
        {"Alerts", new AlertsPage(db)},
        {"Calendar", new CalendarPage(db)},
        {"Metrics & Trends", new MetricsChartsPage(db)},
        {"KPI Trend Engine", new KpiTrendEnginePage(db)},
        {"Service Layer", new ServiceLayerPage(db)},
        {"External Sync", new ExternalSyncReadinessPage(db)},
        {"Release Candidate", new ReleaseCandidatePage(db)},
        {"SOP / Quick Start", new SopCenterPage(db)},
        {"Care Conferences", new CareConferencePage(db)},
        {"Therapy / Rehab", new TherapyRehabPage(db)},
        {"Social Services", new SocialServicesPage(db)},
        {"Environmental Services", new EnvironmentalRoundsPage(db)},
        {"Revenue Cycle", new RevenueCyclePage(db)},
        {"Contracts", new ContractManagementPage(db)},
        {"Department Views", new DepartmentDashboardsPage(db)},
        {"Residents", new ResidentsPage(db)},
        {"Admissions", new AdmissionsPage(db)},
        {"Staffing", new StaffingPage(db)},
        {"Tasks", new TasksPage(db)},
        {"QAPI", new QapiPage(db)},
        {"Budget", new BudgetPage(db)},
        {"Compliance", new CompliancePage(db)},
        {"Huddle", new HuddlePage(db)},
        {"Incidents", new IncidentsPage(db)},
        {"Survey Ready", new SurveyReadinessPage(db)},
        {"Quality", new QualityMeasuresPage(db)},
        {"Managed Care", new ManagedCarePage(db)},
        {"Preparedness", new PreparednessPage(db)},
        {"Infection Control", new InfectionControlPage(db)},
        {"Bed Board", new BedBoardPage(db)},
        {"Transportation", new TransportationPage(db)},
        {"Pharmacy", new PharmacyPage(db)},
        {"Dietary", new DietaryPage(db)},
        {"Document Center", new DocumentCenterPage(db)},
        {"Census Management", new CensusManagementPage(db)},
        {"MDS", new MdsTripleCheckPage(db)},
        {"Survey Cmd", new SurveyCommandCenterPage(db)},
        {"Outbreak Command", new OutbreakCommandPage(db)},
        {"Reports", new ReportsPage(db)},
        {"Backup & Restore", new BackupRestorePage(db)},
        {"Audit Log", new AuditLogPage(db)},
        {"Forms & Validation", new FormsValidationPage(db)},
        {"Workflow Center", new WorkflowCenterPage(db)}
    };

    QStringList allowed;
    if (roleName == "Administrator") {
        for (const auto& pair : pages) allowed << pair.first;
    } else if (roleName == "Director of Nursing") {
        allowed = {"Dashboard","Dashboard Setup","Alerts","Calendar","Metrics & Trends","KPI Trend Engine","Service Layer","External Sync","Release Candidate","SOP / Quick Start","Care Conferences","Therapy / Rehab","Environmental Services","Department Views","Residents","Admissions","Staffing","Tasks","QAPI","Huddle","Incidents","Survey Ready","Quality","Preparedness","Infection Control","Social Services","Pharmacy","Dietary","Document Center","Census Management","MDS","Survey Cmd","Outbreak Command","Reports","Backup & Restore","Audit Log","Forms & Validation","Workflow Center"};
    } else if (roleName == "Admissions Director") {
        allowed = {"Dashboard","Dashboard Setup","Alerts","Calendar","Metrics & Trends","KPI Trend Engine","Service Layer","External Sync","Release Candidate","SOP / Quick Start","Care Conferences","Therapy / Rehab","Social Services","Environmental Services","Department Views","Residents","Admissions","Tasks","Managed Care","Bed Board","Transportation","Document Center","Census Management","MDS","Survey Cmd","Outbreak Command","Reports","Backup & Restore","Audit Log","Forms & Validation","Workflow Center"};
    } else if (roleName == "Staffing Coordinator") {
        allowed = {"Dashboard","Dashboard Setup","Alerts","Calendar","Metrics & Trends","KPI Trend Engine","Service Layer","External Sync","Release Candidate","SOP / Quick Start","Environmental Services","Department Views","Staffing","Tasks","Document Center","Reports","Backup & Restore","Audit Log","Forms & Validation","Workflow Center"};
    } else {
        allowed = {"Dashboard","Dashboard Setup","Alerts","Calendar","Metrics & Trends","KPI Trend Engine","Service Layer","External Sync","Release Candidate","SOP / Quick Start","Care Conferences","Therapy / Rehab","Social Services","Environmental Services","Department Views","Residents","Admissions","Staffing","Quality","Document Center","Census Management","MDS","Survey Cmd","Outbreak Command","Reports","Backup & Restore","Audit Log","Forms & Validation","Workflow Center"};
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

    QString defaultPage = "Dashboard";
    const auto prefRows = db->fetchTable("dashboard_preferences", {"pref_key", "pref_value"});
    QString densityMode = "Comfortable";
    for (const auto& row : prefRows) {
        if (row.value("pref_key") == "default_page") defaultPage = row.value("pref_value");
        if (row.value("pref_key") == "density_mode") densityMode = row.value("pref_value");
    }
    int defaultIndex = 0;
    for (int i = 0; i < nav->count(); ++i) {
        if (nav->item(i)->text() == defaultPage) {
            defaultIndex = i;
            break;
        }
    }
    nav->setCurrentRow(defaultIndex);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    if (densityMode == "Compact") {
        splitter->setSizes(QList<int>{220, 1240});
    } else if (densityMode == "Executive") {
        splitter->setSizes(QList<int>{250, 1210});
    } else {
        splitter->setSizes(QList<int>{240, 1220});
    }

    shellLayout->addWidget(splitter, 1);
    setCentralWidget(shell);
}
