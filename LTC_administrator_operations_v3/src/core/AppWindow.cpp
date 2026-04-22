
#include "AppWindow.h"
#include "../data/DatabaseManager.h"
#include "../ui/pages/AdmissionsPage.h"
#include "../ui/pages/AlertsPage.h"
#include "../ui/pages/BudgetPage.h"
#include "../ui/pages/DashboardPage.h"
#include "../ui/pages/DashboardCustomizePage.h"
#include "../ui/pages/DietaryPage.h"
#include "../ui/pages/DocumentCenterPage.h"
#include "../ui/pages/EnvironmentalRoundsPage.h"
#include "../ui/pages/HuddlePage.h"
#include "../ui/pages/IncidentsPage.h"
#include "../ui/pages/MdsTripleCheckPage.h"
#include "../ui/pages/PharmacyPage.h"
#include "../ui/pages/QapiPage.h"
#include "../ui/pages/QualityMeasuresPage.h"
#include "../ui/pages/ReportsPage.h"
#include "../ui/pages/ResidentsPage.h"
#include "../ui/pages/SocialServicesPage.h"
#include "../ui/pages/StaffingPage.h"
#include "../ui/pages/SurveyReadinessPage.h"
#include "../ui/pages/TasksPage.h"
#include "../ui/pages/TransportationPage.h"
#include "../ui/pages/CalendarPage.h"
#include "../ui/pages/LeadershipRoundsPage.h"
#include "../ui/pages/ExecutiveFollowUpBoardPage.h"
#include "../ui/pages/MorningMeetingBoardPage.h"
#include "../ui/pages/DepartmentPulseBoardPage.h"
#include "../ui/pages/BarrierEscalationBoardPage.h"
#include "../ui/pages/SurveyRecoveryBoardPage.h"
#include "../ui/pages/EvidenceBinderBoardPage.h"
#include "../ui/pages/MockSurveyDrillBoardPage.h"
#include "../ui/pages/SurveyEntranceConferenceBoardPage.h"
#include "../ui/pages/SurveyCommandCenterPage.h"
#include "../ui/pages/AlertsEscalationCenterPage.h"
#include "../ui/pages/SurveyLiveResponseTrackerPage.h"
#include "../ui/pages/SurveyDocumentRequestLogPage.h"
#include "../ui/pages/ResidentTracerManagerPage.h"
#include "../ui/pages/PlanOfCorrectionBuilderPage.h"
#include "../ui/pages/ExecutivePrintExportCenterPage.h"
#include "../ui/pages/TreatmentsPage.h"

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
    setWindowTitle("LTC Administrator Operations Dashboard v81 Alerts & Escalation Center");
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
        "v81 adds an alerts and escalation center so leadership can see overdue, blocked, critical, and due-now issues across the major survey boards in one urgency-focused operational workspace.",
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
    auto* navHint = new QLabel(QString("%1 profile · streamlined module set · pages scroll automatically on smaller screens.").arg(roleName), sidebar);
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
        {"Leadership Rounds", new LeadershipRoundsPage(db)},
        {"Executive Follow-Up", new ExecutiveFollowUpBoardPage(db)},
        {"Morning Meeting", new MorningMeetingBoardPage(db)},
        {"Department Pulse", new DepartmentPulseBoardPage(db)},
        {"Barrier Escalation", new BarrierEscalationBoardPage(db)},
        {"Survey Recovery", new SurveyRecoveryBoardPage(db)},
        {"Evidence Binder", new EvidenceBinderBoardPage(db)},
        {"Mock Survey Drill", new MockSurveyDrillBoardPage(db)},
        {"Entrance Conference", new SurveyEntranceConferenceBoardPage(db)},
        {"Survey Command Center", new SurveyCommandCenterPage(db)},
        {"Alerts & Escalation", new AlertsEscalationCenterPage(db)},
        {"Live Survey Response", new SurveyLiveResponseTrackerPage(db)},
        {"Survey Document Requests", new SurveyDocumentRequestLogPage(db)},
        {"Resident Tracer Manager", new ResidentTracerManagerPage(db)},
        {"Plan of Correction", new PlanOfCorrectionBuilderPage(db)},
        {"Executive Print & Export", new ExecutivePrintExportCenterPage(db)},
        {"Residents", new ResidentsPage(db)},
        {"Admissions", new AdmissionsPage(db)},
        {"HR / Staffing", new StaffingPage(db)},
        {"Tasks", new TasksPage(db)},
        {"Huddle", new HuddlePage(db)},
        {"Social Services", new SocialServicesPage(db)},
        {"Environmental Services", new EnvironmentalRoundsPage(db)},
        {"Medical Records", new PharmacyPage(db)},
        {"MDS", new MdsTripleCheckPage(db)},
        {"DON", new IncidentsPage(db)},
        {"Treatments", new TreatmentsPage(db)},
        {"QAPI", new QapiPage(db)},
        {"Budget", new BudgetPage(db)},
        {"Survey Ready", new SurveyReadinessPage(db)},
        {"Quality", new QualityMeasuresPage(db)},
        {"Transportation", new TransportationPage(db)},
        {"Dietary", new DietaryPage(db)},
        {"Document Center", new DocumentCenterPage(db)},
        {"Reports", new ReportsPage(db)}
    };

    QStringList allowed;
    for (const auto& pair : pages) allowed << pair.first;

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
