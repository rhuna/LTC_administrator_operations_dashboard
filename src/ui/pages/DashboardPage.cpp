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
        "A simplified command center for census movement, staffing coverage, minimum staffing risk, compliance, and operational follow-up.",
        hero);
    subtitle->setObjectName("dashboardSubtitle");
    subtitle->setWordWrap(true);

    auto* snapshot = new QLabel(
        QString("%1 current residents · %2 open staffing assignments · %3 minimum staffing gaps")
            .arg(db->countWhere("residents", "status='Current'"))
            .arg(db->countWhere("staffing_assignments", "status='Open'"))
            .arg(db->countMinimumStaffingGaps()),
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
    kpiGrid->addWidget(new KpiCard("Staffing changes", QString::number(db->countWhere("staffing_changes", "status!='Closed'")), this), 1, 1);
    kpiGrid->addWidget(new KpiCard("Compliance due", QString::number(db->countWhere("compliance_items", "status!='Closed'")), this), 1, 2);
    kpiGrid->addWidget(new KpiCard("Active PIPs", QString::number(db->countWhere("pips", "status='Active'")), this), 2, 0);
    kpiGrid->addWidget(new KpiCard("Open huddle items", QString::number(db->countWhere("huddle_items", "status!='Closed'")), this), 2, 1);
    kpiGrid->addWidget(new KpiCard("Discharged residents", QString::number(db->countWhere("residents", "status='Discharged'")), this), 2, 2);
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
    quickList->addItem(QString("%1 incident(s) still open or under review").arg(db->countWhere("incidents", "status!='Closed'")));
    quickList->addItem(QString("%1 managed-care item(s) at risk or open").arg(db->countWhere("managed_care_items", "status='At Risk' OR status='Open'")));
    quickList->addItem(QString("%1 credentialing item(s) due soon or open").arg(db->countWhere("credentialing_items", "status!='Closed'")));
    quickList->addItem(QString("%1 preparedness item(s) due soon or open").arg(db->countWhere("preparedness_items", "status!='Closed'")));
    quickList->addItem(QString("%1 infection-control item(s) open or on watch").arg(db->countWhere("infection_control_items", "status='Open' OR status='Watch'")));
    quickLayout->addWidget(quickHint);
    quickLayout->addWidget(quickList);

    lowerRow->addWidget(actionBox, 3);
    lowerRow->addWidget(quickBox, 2);
    root->addLayout(lowerRow);
    root->addStretch();
}
