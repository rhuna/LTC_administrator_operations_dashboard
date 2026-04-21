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
        "A simplified command center for census, referral readiness, staffing coverage, survey work, outbreak workload, quality analytics, documents, and due-soon follow-up.",
        hero);
    subtitle->setObjectName("dashboardSubtitle");
    subtitle->setWordWrap(true);

    auto* snapshot = new QLabel(
        QString("%1 residents · %2 waitlist referrals · %3 open staffing · %4 minimum gaps · %5 overdue alerts · %6 off-target quality measures")
            .arg(db->countWhere("residents", "status='Current'"))
            .arg(db->countWhere("admissions", "status!='Admitted' AND status!='Discharged'"))
            .arg(db->countWhere("staffing_assignments", "status='Open'"))
            .arg(db->countMinimumStaffingGaps())
            .arg(db->overdueAlertCount())
            .arg(db->countWhere("quality_measures", "status='Off Target'")),
        hero);
    snapshot->setObjectName("dashboardSnapshot");

    heroLayout->addWidget(title);
    heroLayout->addWidget(subtitle);
    heroLayout->addWidget(snapshot);
    root->addWidget(hero);

    auto* summaryStrip = new QFrame(this);
    summaryStrip->setObjectName("summaryStrip");
    auto* summaryLayout = new QHBoxLayout(summaryStrip);
    summaryLayout->setContentsMargins(16, 12, 16, 12);
    summaryLayout->setSpacing(18);
    auto addSummary = [&](const QString& label, const QString& value) {
        auto* wrap = new QVBoxLayout();
        auto* l = new QLabel(label, summaryStrip);
        l->setObjectName("panelHint");
        auto* v = new QLabel(value, summaryStrip);
        v->setStyleSheet("font-size:20px; font-weight:700; color:#102a43;");
        wrap->addWidget(l);
        wrap->addWidget(v);
        summaryLayout->addLayout(wrap);
    };
    addSummary("Ready referrals", QString::number(db->countWhere("admissions", "status='Ready'")));
    addSummary("Needs docs", QString::number(db->countWhere("admissions", "status='Needs Docs'")));
    addSummary("Open beds / turnovers", QString::number(db->countWhere("bed_board", "status!='Closed'")));
    addSummary("Open incidents", QString::number(db->countWhere("incidents", "status!='Closed'")));
    addSummary("Open quality follow-ups", QString::number(db->countWhere("quality_followups", "status!='Closed' AND status!='Complete'")));
    addSummary("Outbreak items", QString::number(db->countWhere("outbreak_items", "status!='Closed' AND status!='Complete'")));
    summaryLayout->addStretch();
    root->addWidget(summaryStrip);

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
    kpiGrid->addWidget(new KpiCard("Referrals ready", QString::number(db->countWhere("admissions", "status='Ready'")), this), 7, 0);
    kpiGrid->addWidget(new KpiCard("Needs documents", QString::number(db->countWhere("admissions", "status='Needs Docs'")), this), 7, 1);
    kpiGrid->addWidget(new KpiCard("Outbreak items", QString::number(db->countWhere("outbreak_items", "status!='Closed' AND status!='Complete'")), this), 7, 2);
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
    quickList->addItem(QString("%1 assignment(s) are still open").arg(db->countWhere("staffing_assignments", "status='Open'")));
    quickList->addItem(QString("%1 staffing group(s) are below minimum coverage").arg(db->countMinimumStaffingGaps()));
    quickList->addItem(QString("%1 referral(s) are marked ready for admit").arg(db->countWhere("admissions", "status='Ready'")));
    quickList->addItem(QString("%1 referral(s) still need documents").arg(db->countWhere("admissions", "status='Needs Docs'")));
    quickList->addItem(QString("%1 document item(s) remain open").arg(db->countWhere("document_items", "status!='Closed'")));
    quickList->addItem(QString("%1 MDS / triple-check item(s) remain open").arg(db->countWhere("mds_items", "status!='Closed' AND status!='Complete'")));
    quickList->addItem(QString("%1 survey command item(s) remain open").arg(db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'")));
    quickList->addItem(QString("%1 outbreak item(s) remain open").arg(db->countWhere("outbreak_items", "status!='Closed' AND status!='Complete'")));
    quickList->addItem(QString("%1 quality measure(s) are off target").arg(db->countWhere("quality_measures", "status='Off Target'")));
    quickList->addItem(QString("%1 overdue alert(s) need immediate attention").arg(db->overdueAlertCount()));
    quickList->addItem("Reports workspace can export a daily summary, census CSV, and staffing CSV.");
    quickLayout->addWidget(quickHint);
    quickLayout->addWidget(quickList);

    lowerRow->addWidget(actionBox, 3);
    lowerRow->addWidget(quickBox, 2);
    root->addLayout(lowerRow);
    root->addStretch();
}
