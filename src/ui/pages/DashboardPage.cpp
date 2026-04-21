#include "DashboardPage.h"
#include "../../data/DatabaseManager.h"
#include "../widgets/KpiCard.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

DashboardPage::DashboardPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(16);

    auto* hero = new QGroupBox(this);
    hero->setObjectName("heroPanel");
    auto* heroLayout = new QVBoxLayout(hero);
    heroLayout->setContentsMargins(20, 18, 20, 18);
    heroLayout->setSpacing(6);

    auto* title = new QLabel("Executive dashboard", hero);
    title->setObjectName("dashboardTitle");
    auto* subtitle = new QLabel(
        "A simplified command center for census movement, referral waitlist intake, document intake, staffing coverage, bed availability, MDS / ARD / triple-check oversight, survey command follow-up, outbreak response workload, due-soon alerts, and operational follow-up.",
        hero);
    subtitle->setObjectName("dashboardSubtitle");
    subtitle->setWordWrap(true);

    auto* snapshot = new QLabel(
        QString("%1 current residents · %2 open staffing assignments · %3 minimum staffing gaps · %4 uncovered minimum hours · %5 transport items · %6 pharmacy items · %7 dietary items · %8 open tasks · %9 census events · %10 MDS / triple-check items · %11 survey command items · %12 outbreak items · %13 overdue alerts · %14 document items · %15 waitlist referrals")
            .arg(db->countWhere("residents", "status='Current'"))
            .arg(db->countWhere("staffing_assignments", "status='Open'"))
            .arg(db->countMinimumStaffingGaps())
            .arg(db->estimatedMinimumHoursGap())
            .arg(db->countWhere("transport_items", "status!='Returned' AND status!='Closed'"))
            .arg(db->countWhere("pharmacy_items", "status='Open' OR status='Watch' OR status='In Progress'"))
            .arg(db->countWhere("dietary_items", "status='Open' OR status='Watch' OR status='In Progress'"))
            .arg(db->countWhere("tasks", "status!='Complete'"))
            .arg(db->countWhere("census_events", "status!='Closed'"))
            .arg(db->countWhere("mds_items", "status!='Closed' AND status!='Complete'"))
            .arg(db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'"))
            .arg(db->countWhere("outbreak_items", "status!='Closed' AND status!='Complete'"))
            .arg(db->overdueAlertCount())
            .arg(db->countWhere("document_items", "status!='Closed'"))
            .arg(db->countWhere("admissions", "status!='Admitted' AND status!='Discharged'")),
        hero);
    snapshot->setObjectName("dashboardSnapshot");

    heroLayout->addWidget(title);
    heroLayout->addWidget(subtitle);
    heroLayout->addWidget(snapshot);
    root->addWidget(hero);

    auto* kpiGrid = new QGridLayout();
    kpiGrid->setHorizontalSpacing(12);
    kpiGrid->setVerticalSpacing(12);
    kpiGrid->addWidget(new KpiCard("Current residents", QString::number(db->countWhere("residents", "status='Current'")), this), 0, 0);
    kpiGrid->addWidget(new KpiCard("Pending admissions", QString::number(db->countWhere("admissions", "status='Pending' OR status='Accepted'")), this), 0, 1);
    kpiGrid->addWidget(new KpiCard("Open staffing", QString::number(db->countWhere("staffing_assignments", "status='Open'")), this), 0, 2);
    kpiGrid->addWidget(new KpiCard("Minimum staffing gaps", QString::number(db->countMinimumStaffingGaps()), this), 1, 0);
    kpiGrid->addWidget(new KpiCard("Uncovered minimum hours", QString::number(db->estimatedMinimumHoursGap()), this), 1, 1);
    kpiGrid->addWidget(new KpiCard("Est. nursing HPRD", QString::number(db->estimatedNursingHprd(), 'f', 2), this), 1, 2);
    kpiGrid->addWidget(new KpiCard("Agency assignments", QString::number(db->countWhere("staffing_assignments", "employee_name LIKE '%Agency%' OR employee_name LIKE '%Pool%'")), this), 2, 0);
    kpiGrid->addWidget(new KpiCard("Filled assignments", QString::number(db->countWhere("staffing_assignments", "status='Filled'")), this), 2, 1);
    kpiGrid->addWidget(new KpiCard("Open beds / turnovers", QString::number(db->countWhere("bed_board", "status!='Closed'")), this), 2, 2);
    kpiGrid->addWidget(new KpiCard("Transport items", QString::number(db->countWhere("transport_items", "status!='Returned' AND status!='Closed'")), this), 3, 0);
    kpiGrid->addWidget(new KpiCard("Pharmacy items", QString::number(db->countWhere("pharmacy_items", "status='Open' OR status='Watch' OR status='In Progress'")), this), 3, 1);
    kpiGrid->addWidget(new KpiCard("Dietary items", QString::number(db->countWhere("dietary_items", "status='Open' OR status='Watch' OR status='In Progress'")), this), 3, 2);
    kpiGrid->addWidget(new KpiCard("Open tasks", QString::number(db->countWhere("tasks", "status!='Complete'")), this), 4, 0);
    kpiGrid->addWidget(new KpiCard("Census events", QString::number(db->countWhere("census_events", "status!='Closed'")), this), 4, 1);
    kpiGrid->addWidget(new KpiCard("Open incidents", QString::number(db->countWhere("incidents", "status!='Closed'")), this), 4, 2);
    kpiGrid->addWidget(new KpiCard("MDS / triple-check", QString::number(db->countWhere("mds_items", "status!='Closed' AND status!='Complete'")), this), 5, 0);
    kpiGrid->addWidget(new KpiCard("Survey command", QString::number(db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'")), this), 5, 1);
    kpiGrid->addWidget(new KpiCard("Overdue alerts", QString::number(db->overdueAlertCount()), this), 5, 2);
    kpiGrid->addWidget(new KpiCard("Due soon alerts", QString::number(db->dueSoonAlertCount()), this), 6, 0);
    kpiGrid->addWidget(new KpiCard("Document items", QString::number(db->countWhere("document_items", "status!='Closed'")), this), 6, 1);
    kpiGrid->addWidget(new KpiCard("Waitlist referrals", QString::number(db->countWhere("admissions", "status!='Admitted' AND status!='Discharged'")), this), 6, 2);
    kpiGrid->addWidget(new KpiCard("Outbreak items", QString::number(db->countWhere("outbreak_items", "status!='Closed' AND status!='Complete'")), this), 7, 0);
    root->addLayout(kpiGrid);

    auto* lowerRow = new QHBoxLayout();
    lowerRow->setSpacing(16);

    auto* actionBox = new QGroupBox("Administrator action center", this);
    auto* actionLayout = new QVBoxLayout(actionBox);
    auto* actionHint = new QLabel("Short list of the highest-value follow-up items.", actionBox);
    actionHint->setObjectName("panelHint");
    actionHint->setWordWrap(true);
    auto* list = new QListWidget(actionBox);
    list->setObjectName("actionList");
    for (const auto& item : db->actionCenterItems()) {
        list->addItem(item.first + " — " + item.second);
    }
    actionLayout->addWidget(actionHint);
    actionLayout->addWidget(list);

    auto* quickBox = new QGroupBox("Today at a glance", this);
    auto* quickLayout = new QVBoxLayout(quickBox);
    auto* quickHint = new QLabel("Use this to orient quickly before moving into a detailed module.", quickBox);
    quickHint->setObjectName("panelHint");
    quickHint->setWordWrap(true);
    auto* quickList = new QListWidget(quickBox);
    quickList->setObjectName("actionList");
    quickList->addItem(QString("%1 assignment(s) still marked open").arg(db->countWhere("staffing_assignments", "status='Open'")));
    quickList->addItem(QString("%1 minimum staffing group(s) are below required coverage").arg(db->countMinimumStaffingGaps()));
    quickList->addItem(QString("%1 assignment(s) are currently agency-covered").arg(db->countWhere("staffing_assignments", "employee_name LIKE '%Agency%' OR employee_name LIKE '%Pool%'")));
    quickList->addItem(QString("Estimated nursing HPRD is %1").arg(QString::number(db->estimatedNursingHprd(), 'f', 2)));
    quickList->addItem(QString("%1 minimum uncovered staffing hours remain").arg(db->estimatedMinimumHoursGap()));
    quickList->addItem(QString("%1 bed-board or room-turnover item(s) remain open").arg(db->countWhere("bed_board", "status!='Closed'")));
    quickList->addItem(QString("%1 census event(s) are still open, planned, or under review").arg(db->countWhere("census_events", "status!='Closed'")));
    quickList->addItem(QString("%1 incident(s) still open or under review").arg(db->countWhere("incidents", "status!='Closed'")));
    quickList->addItem(QString("%1 managed-care item(s) at risk or open").arg(db->countWhere("managed_care_items", "status='At Risk' OR status='Open'")));
    quickList->addItem(QString("%1 credentialing item(s) due soon or open").arg(db->countWhere("credentialing_items", "status!='Closed'")));
    quickList->addItem(QString("%1 preparedness item(s) due soon or open").arg(db->countWhere("preparedness_items", "status!='Closed'")));
    quickList->addItem(QString("%1 infection-control item(s) open or on watch").arg(db->countWhere("infection_control_items", "status='Open' OR status='Watch'")));
    quickList->addItem(QString("%1 transportation or outside-appointment item(s) still need follow-up").arg(db->countWhere("transport_items", "status!='Returned' AND status!='Closed'")));
    quickList->addItem(QString("%1 waitlist referral(s) are still pending review or admit").arg(db->countWhere("admissions", "status!='Admitted' AND status!='Discharged'")));
    quickList->addItem(QString("%1 document item(s) are open or still being assembled").arg(db->countWhere("document_items", "status!='Closed'")));
    quickList->addItem(QString("%1 MDS / ARD / triple-check item(s) remain open, in progress, or on watch").arg(db->countWhere("mds_items", "status!='Closed' AND status!='Complete'")));
    quickList->addItem(QString("%1 survey command item(s) are still open for mock survey, evidence, or plan-of-correction follow-up").arg(db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'")));
    quickList->addItem(QString("%1 outbreak-response item(s) remain open, in progress, or on watch").arg(db->countWhere("outbreak_items", "status!='Closed' AND status!='Complete'")));
    quickList->addItem(QString("%1 pharmacy or medication-system item(s) are open or on watch").arg(db->countWhere("pharmacy_items", "status='Open' OR status='Watch' OR status='In Progress'")));
    quickList->addItem(QString("%1 dietary or nutrition item(s) are open or on watch").arg(db->countWhere("dietary_items", "status='Open' OR status='Watch' OR status='In Progress'")));
    quickList->addItem(QString("%1 overdue alert(s) need immediate attention across due-date driven modules").arg(db->overdueAlertCount()));
    quickList->addItem(QString("%1 due-soon alert(s) are approaching in the next three days").arg(db->dueSoonAlertCount()));
    quickList->addItem("Reports workspace can export a daily summary, census CSV, and staffing CSV for leadership review");
    quickLayout->addWidget(quickHint);
    quickLayout->addWidget(quickList);

    lowerRow->addWidget(actionBox, 3);
    lowerRow->addWidget(quickBox, 2);
    root->addLayout(lowerRow);
    root->addStretch();
}
