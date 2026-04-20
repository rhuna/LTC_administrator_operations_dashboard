#include "DashboardPage.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

#include "data/DatabaseManager.h"
#include "ui/widgets/KpiCard.h"

DashboardPage::DashboardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), db_(db), subtitleLabel_(new QLabel(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Executive Dashboard", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    subtitleLabel_->setObjectName("SectionTitle");
    root->addWidget(subtitleLabel_);

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
    admissionsCard_ = new KpiCard("Pending Admissions", "0", this);
    dischargeCard_ = new KpiCard("Planned Discharges", "0", this);
    agencyCard_ = new KpiCard("Agency Shifts", "0", this);
    qapiCard_ = new KpiCard("Active PIPs", "0", this);
    overtimeCard_ = new KpiCard("OT Hours", "0.0", this);
    varianceCard_ = new KpiCard("Labor Variance", "$0", this);
    complianceCard_ = new KpiCard("Compliance Due", "0", this);
    staffingChangeCard_ = new KpiCard("Staffing Changes", "0", this);
    huddleCard_ = new KpiCard("Open Huddle Items", "0", this);
    qualityCard_ = new KpiCard("Quality Measures Off Target", "0", this);
    trainingCard_ = new KpiCard("Training Due", "0", this);
    emergencyCard_ = new KpiCard("Preparedness Items Due", "0", this);
    infectionCard_ = new KpiCard("Infection Control Open", "0", this);
    cardsRow2->addWidget(hprdCard_);
    cardsRow2->addWidget(surveyRiskCard_);
    cardsRow2->addWidget(admissionsCard_);
    cardsRow2->addWidget(dischargeCard_);
    cardsRow2->addWidget(agencyCard_);
    cardsRow2->addWidget(qapiCard_);
    cardsRow2->addWidget(overtimeCard_);
    cardsRow2->addWidget(varianceCard_);
    cardsRow2->addWidget(complianceCard_);
    cardsRow2->addWidget(staffingChangeCard_);
    cardsRow2->addWidget(huddleCard_);
    cardsRow2->addWidget(qualityCard_);
    cardsRow2->addWidget(trainingCard_);
    cardsRow2->addWidget(emergencyCard_);
    cardsRow2->addWidget(infectionCard_);
    root->addLayout(cardsRow2);

    auto* prioritiesBox = new QGroupBox("Administrator Action Center", this);
    auto* prioritiesLayout = new QVBoxLayout(prioritiesBox);
    priorityList_ = new QListWidget(prioritiesBox);
    prioritiesLayout->addWidget(priorityList_);
    root->addWidget(prioritiesBox, 1);

    refresh();
}

void DashboardPage::refresh() {
    const int census = db_->currentCensus();
    const int capacity = db_->bedCapacity();
    const double occupancy = capacity > 0 ? (100.0 * census / capacity) : 0.0;

    subtitleLabel_->setText(QString("%1 | Beds: %2 | High Priority Tasks: %3 | Active PIPs: %4 | Compliance Due: %5 | Staffing Changes: %6 | Huddle Items: %7 | QM Off Target: %8 | Training Due: %9 | Preparedness Due: %10 | Infection Control Open: %11")
                                .arg(db_->facilityName())
                                .arg(capacity)
                                .arg(db_->highPriorityTasks())
                                .arg(db_->activeQapiProjects())
                                .arg(db_->complianceItemsDueSoon())
                                .arg(db_->pendingStaffingChanges())
                                .arg(db_->openHuddleItems())
                                .arg(db_->qualityMeasuresOffTarget())
                                .arg(db_->trainingItemsDueSoon())
                                .arg(db_->emergencyPreparednessItemsDueSoon())
                                .arg(db_->infectionControlItemsOpen()));

    censusCard_->setValue(QString::number(census));
    occupancyCard_->setValue(QString::number(occupancy, 'f', 1) + "%");
    incidentsCard_->setValue(QString::number(db_->openIncidents()));
    tasksCard_->setValue(QString::number(db_->openTasks()));
    hprdCard_->setValue(QString::number(db_->averageStaffHoursPerResidentDay(), 'f', 2));
    surveyRiskCard_->setValue(QString::number(db_->surveyItemsAtRisk()));
    admissionsCard_->setValue(QString::number(db_->pendingAdmissions()));
    dischargeCard_->setValue(QString::number(db_->plannedDischarges()));
    agencyCard_->setValue(QString::number(db_->agencyShifts()));
    qapiCard_->setValue(QString::number(db_->activeQapiProjects()));
    overtimeCard_->setValue(QString::number(db_->totalOvertimeHours(), 'f', 1));
    varianceCard_->setValue(QString("$%1").arg(QString::number(db_->monthlyLaborBudgetVariance(), 'f', 0)));
    complianceCard_->setValue(QString::number(db_->complianceItemsDueSoon()));
    staffingChangeCard_->setValue(QString::number(db_->pendingStaffingChanges()));
    huddleCard_->setValue(QString::number(db_->openHuddleItems()));
    qualityCard_->setValue(QString::number(db_->qualityMeasuresOffTarget()));
    trainingCard_->setValue(QString::number(db_->trainingItemsDueSoon()));
    emergencyCard_->setValue(QString::number(db_->emergencyPreparednessItemsDueSoon()));
    infectionCard_->setValue(QString::number(db_->infectionControlItemsOpen()));

    priorityList_->clear();
    for (const auto& item : db_->dashboardFocusItems()) {
        priorityList_->addItem(item);
    }
}
