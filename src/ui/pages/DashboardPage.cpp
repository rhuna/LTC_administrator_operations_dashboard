#include "DashboardPage.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

#include "data/DatabaseManager.h"
#include "ui/widgets/KpiCard.h"

DashboardPage::DashboardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), db_(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Executive Dashboard", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* cardsRow1 = new QHBoxLayout();
    cardsRow1->setSpacing(14);
    censusCard_ = new KpiCard("Current Census", "0", this);
    occupancyCard_ = new KpiCard("Occupancy", "0%", this);
    incidentsCard_ = new KpiCard("Open Incidents", "0", this);
    tasksCard_ = new KpiCard("Open Tasks", "0", this);
    cardsRow1->addWidget(censusCard_);
    cardsRow1->addWidget(occupancyCard_);
    cardsRow1->addWidget(incidentsCard_);
    cardsRow1->addWidget(tasksCard_);
    root->addLayout(cardsRow1);

    auto* cardsRow2 = new QHBoxLayout();
    cardsRow2->setSpacing(14);
    hprdCard_ = new KpiCard("Avg HPRD", "0.0", this);
    surveyRiskCard_ = new KpiCard("Survey Items At Risk", "0", this);
    cardsRow2->addWidget(hprdCard_);
    cardsRow2->addWidget(surveyRiskCard_);
    cardsRow2->addStretch();
    root->addLayout(cardsRow2);

    auto* prioritiesBox = new QGroupBox("Administrator Focus Items", this);
    auto* prioritiesLayout = new QVBoxLayout(prioritiesBox);
    priorityList_ = new QListWidget(prioritiesBox);
    priorityList_->addItem("Review open incident follow-up before morning standup.");
    priorityList_->addItem("Verify agency utilization and overtime trends for nursing.");
    priorityList_->addItem("Prepare at-risk survey binders for accident/supervision and life safety.");
    priorityList_->addItem("Complete overdue QAPI packet tasks and leadership sign-off.");
    prioritiesLayout->addWidget(priorityList_);
    root->addWidget(prioritiesBox, 1);

    refresh();
}

void DashboardPage::refresh() {
    const int census = db_->currentCensus();
    const int capacity = db_->bedCapacity();
    const double occupancy = capacity > 0 ? (100.0 * census / capacity) : 0.0;

    censusCard_->setValue(QString::number(census));
    occupancyCard_->setValue(QString::number(occupancy, 'f', 1) + "%");
    incidentsCard_->setValue(QString::number(db_->openIncidents()));
    tasksCard_->setValue(QString::number(db_->openTasks()));
    hprdCard_->setValue(QString::number(db_->averageStaffHoursPerResidentDay(), 'f', 2));
    surveyRiskCard_->setValue(QString::number(db_->surveyItemsAtRisk()));
}
