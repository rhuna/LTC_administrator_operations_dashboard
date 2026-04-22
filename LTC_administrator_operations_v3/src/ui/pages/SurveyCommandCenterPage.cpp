#include "SurveyCommandCenterPage.h"
#include "../../data/DatabaseManager.h"
#include "../widgets/KpiCard.h"

#include <QAbstractItemView>
#include <QDate>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTime>
#include <QVBoxLayout>

SurveyCommandCenterPage::SurveyCommandCenterPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    const QString today = QDate::currentDate().toString("yyyy-MM-dd");

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(16);

    auto* hero = new QGroupBox(this);
    auto* heroLayout = new QVBoxLayout(hero);
    heroLayout->setContentsMargins(20, 18, 20, 18);
    heroLayout->setSpacing(8);

    auto* heading = new QLabel("Survey Command Center", hero);
    heading->setObjectName("dashboardTitle");

    auto* subtitle = new QLabel(
        "A live survey-operations control room that rolls Survey Recovery, Evidence Binder, Mock Survey Drill, Entrance Conference, Barrier Escalation, Executive Follow-Up, live survey requests, document pulls, resident tracers, plan-of-correction work, executive print/export packets, and alert-center escalation items into one leadership view. Use it to spot what is overdue, blocked, urgent, missing, and still not ready before or during survey activity.",
        hero);
    subtitle->setObjectName("dashboardSubtitle");
    subtitle->setWordWrap(true);

    const int recoveryOpen = db->countWhere("survey_recovery_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence'");
    const int binderOpen = db->countWhere("evidence_binder_items", "status='Open' OR status='Collecting'");
    const int drillOpen = db->countWhere("mock_survey_drills", "status='Open' OR status='Assigned' OR status='In Drill'");
    const int entranceOpen = db->countWhere("survey_entrance_conference_items", "status='Open' OR status='Assigned' OR status='Assembling'");
    const int barrierOpen = db->countWhere("barrier_escalations", "status='Open' OR status='Assigned' OR status='Waiting'");
    const int execOpen = db->countWhere("executive_followups", "status='Open' OR status='In Progress' OR status='Watch' OR status='Blocked'");
    const int liveOpen = db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'");
    const int documentOpen = db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'");
    const int tracerOpen = db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated'");
    const int pocOpen = db->countWhere("plan_of_correction_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review'");
    const int exportOpen = db->countWhere("executive_export_packets", "status='Drafting' OR status='Waiting on Input' OR status='Ready'");
    const int alertOpen = db->countWhere("alerts_escalation_items", "status='Open' OR status='Due Today' OR status='Blocked'");
    const int commandOpen = recoveryOpen + binderOpen + drillOpen + entranceOpen + barrierOpen + execOpen + liveOpen + documentOpen + tracerOpen + pocOpen + exportOpen + alertOpen;

    const int overdueRecovery = db->countWhere("survey_recovery_items", QString("(status='Open' OR status='In Progress' OR status='Awaiting Evidence') AND due_date < '%1'").arg(today));
    const int dueBinder = db->countWhere("evidence_binder_items", QString("(status='Open' OR status='Collecting') AND due_date <= '%1'").arg(today));
    const int dueDrills = db->countWhere("mock_survey_drills", QString("(status='Open' OR status='Assigned' OR status='In Drill') AND due_date <= '%1'").arg(today));
    const int dueEntrance = db->countWhere("survey_entrance_conference_items", QString("(status='Open' OR status='Assigned' OR status='Assembling') AND due_date <= '%1'").arg(today));
    const int urgentBarriers = db->countWhere("barrier_escalations", "(status='Open' OR status='Assigned' OR status='Waiting') AND (severity='High' OR severity='Critical')");
    const int blockedExec = db->countWhere("executive_followups", "status='Blocked'");
    const QString nowTime = QTime::currentTime().toString("HH:mm");
    const int liveDueSoon = db->countWhere("survey_live_requests", QString("(status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon') AND request_date='%1' AND due_time <= '%2'").arg(today, nowTime));
    const int liveOverdue = db->countWhere("survey_live_requests", QString("(status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon') AND request_date < '%1'").arg(today))
        + db->countWhere("survey_live_requests", QString("(status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon') AND request_date='%1' AND due_time < '%2'").arg(today, nowTime));
    const int documentDueToday = db->countWhere("survey_document_requests", QString("(status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing') AND due_date <= '%1'").arg(today));
    const int documentMissing = db->countWhere("survey_document_requests", "status='Missing'");
    const int tracerHighRisk = db->countWhere("resident_tracer_items", "(status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated') AND (risk_level='High' OR risk_level='Critical')");
    const int tracerFollowup = db->countWhere("resident_tracer_items", "status='Needs Follow-Up' OR status='Escalated'");
    const int pocOverdue = db->countWhere("plan_of_correction_items", QString("(status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review') AND due_date < '%1'").arg(today));
    const int pocEvidence = db->countWhere("plan_of_correction_items", "status='Awaiting Evidence' OR status='Under Review'");
    const int exportDue = db->countWhere("executive_export_packets", QString("(status='Drafting' OR status='Waiting on Input' OR status='Ready') AND due_date <= '%1'").arg(today));
    const int exportReady = db->countWhere("executive_export_packets", "status='Ready'");
    const int alertCritical = db->countWhere("alerts_escalation_items", "(status='Open' OR status='Due Today' OR status='Blocked') AND severity='Critical'");
    const int alertBlocked = db->countWhere("alerts_escalation_items", "status='Blocked'");
    const int alertDue = db->countWhere("alerts_escalation_items", QString("(status='Open' OR status='Due Today' OR status='Blocked') AND due_date <= '%1'").arg(today));
    const int dueNow = overdueRecovery + dueBinder + dueDrills + dueEntrance + urgentBarriers + blockedExec + liveDueSoon + liveOverdue + documentDueToday + documentMissing + tracerHighRisk + tracerFollowup + pocOverdue + pocEvidence + exportDue + exportReady + alertCritical + alertBlocked + alertDue;

    const int totalCompleteUnits = db->countWhere("evidence_binder_items", "status='Ready' OR readiness='Ready'")
        + db->countWhere("mock_survey_drills", "status='Complete'")
        + db->countWhere("survey_entrance_conference_items", "status='Ready'")
        + db->countWhere("survey_recovery_items", "status='Complete'")
        + db->countWhere("barrier_escalations", "status='Removed'")
        + db->countWhere("executive_followups", "status='Complete'")
        + db->countWhere("survey_live_requests", "status='Delivered' OR status='Closed'")
        + db->countWhere("survey_document_requests", "status='Delivered' OR status='Closed'")
        + db->countWhere("resident_tracer_items", "status='Resolved' OR status='Closed'")
        + db->countWhere("plan_of_correction_items", "status='Complete' OR status='Submitted'")
        + db->countWhere("executive_export_packets", "status='Exported' OR status='Printed' OR status='Delivered'")
        + db->countWhere("alerts_escalation_items", "status='Resolved'");
    const int readinessBase = commandOpen + totalCompleteUnits;
    const int readinessScore = readinessBase > 0 ? (totalCompleteUnits * 100) / readinessBase : 100;

    auto* snapshot = new QLabel(
        QString("%1 active survey-readiness item(s) · %2 due-now / blocked item(s) · %3% readiness score")
            .arg(commandOpen)
            .arg(dueNow)
            .arg(readinessScore),
        hero);
    snapshot->setObjectName("dashboardSnapshot");

    heroLayout->addWidget(heading);
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

    addSummary("Recovery", QString::number(recoveryOpen));
    addSummary("Binder", QString::number(binderOpen));
    addSummary("Drills", QString::number(drillOpen));
    addSummary("Entrance", QString::number(entranceOpen));
    addSummary("Barriers", QString::number(barrierOpen));
    addSummary("Exec follow-up", QString::number(execOpen));
    addSummary("Live requests", QString::number(liveOpen));
    addSummary("Doc pulls", QString::number(documentOpen));
    addSummary("Doc missing", QString::number(documentMissing));
    addSummary("Tracers", QString::number(tracerOpen));
    addSummary("Tracer risk", QString::number(tracerHighRisk));
    addSummary("POC open", QString::number(pocOpen));
    addSummary("POC due", QString::number(pocOverdue + pocEvidence));
    addSummary("Packets", QString::number(exportOpen));
    addSummary("Packet due", QString::number(exportDue));
    addSummary("Alert ctr.", QString::number(alertOpen));
    addSummary("Alert crit.", QString::number(alertCritical));
    addSummary("Due now", QString::number(dueNow));
    addSummary("Ready score", QString("%1%").arg(readinessScore));
    summaryLayout->addStretch();
    root->addWidget(summaryStrip);

    auto* kpiGrid = new QGridLayout();
    kpiGrid->setHorizontalSpacing(12);
    kpiGrid->setVerticalSpacing(12);
    kpiGrid->addWidget(new KpiCard("Active survey items", QString::number(commandOpen), this), 0, 0);
    kpiGrid->addWidget(new KpiCard("Due-now items", QString::number(dueNow), this), 0, 1);
    kpiGrid->addWidget(new KpiCard("Readiness score", QString("%1%").arg(readinessScore), this), 0, 2);
    kpiGrid->addWidget(new KpiCard("Recovery open", QString::number(recoveryOpen), this), 1, 0);
    kpiGrid->addWidget(new KpiCard("Overdue recovery", QString::number(overdueRecovery), this), 1, 1);
    kpiGrid->addWidget(new KpiCard("Recovery complete", QString::number(db->countWhere("survey_recovery_items", "status='Complete'")), this), 1, 2);
    kpiGrid->addWidget(new KpiCard("Binder open", QString::number(binderOpen), this), 2, 0);
    kpiGrid->addWidget(new KpiCard("Binder due now", QString::number(dueBinder), this), 2, 1);
    kpiGrid->addWidget(new KpiCard("Binder ready", QString::number(db->countWhere("evidence_binder_items", "status='Ready' OR readiness='Ready'")), this), 2, 2);
    kpiGrid->addWidget(new KpiCard("Drills open", QString::number(drillOpen), this), 3, 0);
    kpiGrid->addWidget(new KpiCard("Drills due", QString::number(dueDrills), this), 3, 1);
    kpiGrid->addWidget(new KpiCard("Drills complete", QString::number(db->countWhere("mock_survey_drills", "status='Complete'")), this), 3, 2);
    kpiGrid->addWidget(new KpiCard("Entrance open", QString::number(entranceOpen), this), 4, 0);
    kpiGrid->addWidget(new KpiCard("Entrance due", QString::number(dueEntrance), this), 4, 1);
    kpiGrid->addWidget(new KpiCard("Entrance ready", QString::number(db->countWhere("survey_entrance_conference_items", "status='Ready'")), this), 4, 2);
    kpiGrid->addWidget(new KpiCard("Barrier items", QString::number(barrierOpen), this), 5, 0);
    kpiGrid->addWidget(new KpiCard("Urgent barriers", QString::number(urgentBarriers), this), 5, 1);
    kpiGrid->addWidget(new KpiCard("Barriers removed", QString::number(db->countWhere("barrier_escalations", "status='Removed'")), this), 5, 2);
    kpiGrid->addWidget(new KpiCard("Live requests", QString::number(liveOpen), this), 6, 0);
    kpiGrid->addWidget(new KpiCard("Requests due soon", QString::number(liveDueSoon), this), 6, 1);
    kpiGrid->addWidget(new KpiCard("Requests overdue", QString::number(liveOverdue), this), 6, 2);
    kpiGrid->addWidget(new KpiCard("Doc requests", QString::number(documentOpen), this), 7, 0);
    kpiGrid->addWidget(new KpiCard("Doc due today", QString::number(documentDueToday), this), 7, 1);
    kpiGrid->addWidget(new KpiCard("Doc missing", QString::number(documentMissing), this), 7, 2);
    kpiGrid->addWidget(new KpiCard("Tracer items", QString::number(tracerOpen), this), 8, 0);
    kpiGrid->addWidget(new KpiCard("High-risk tracers", QString::number(tracerHighRisk), this), 8, 1);
    kpiGrid->addWidget(new KpiCard("Tracer follow-up", QString::number(tracerFollowup), this), 8, 2);
    kpiGrid->addWidget(new KpiCard("POC open", QString::number(pocOpen), this), 9, 0);
    kpiGrid->addWidget(new KpiCard("POC overdue", QString::number(pocOverdue), this), 9, 1);
    kpiGrid->addWidget(new KpiCard("Awaiting evidence", QString::number(pocEvidence), this), 9, 2);
    kpiGrid->addWidget(new KpiCard("Export packets", QString::number(exportOpen), this), 10, 0);
    kpiGrid->addWidget(new KpiCard("Packets due now", QString::number(exportDue), this), 10, 1);
    kpiGrid->addWidget(new KpiCard("Ready to print", QString::number(exportReady), this), 10, 2);
    kpiGrid->addWidget(new KpiCard("Alert-center open", QString::number(alertOpen), this), 11, 0);
    kpiGrid->addWidget(new KpiCard("Critical alerts", QString::number(alertCritical), this), 11, 1);
    kpiGrid->addWidget(new KpiCard("Blocked alerts", QString::number(alertBlocked), this), 11, 2);
    root->addLayout(kpiGrid);

    auto* lowerRow = new QHBoxLayout();
    lowerRow->setSpacing(16);

    auto* urgentBox = new QGroupBox("Urgent actions", this);
    auto* urgentLayout = new QVBoxLayout(urgentBox);
    auto* urgentHint = new QLabel("The highest-friction survey items leadership should clear first.", urgentBox);
    urgentHint->setObjectName("panelHint");
    urgentHint->setWordWrap(true);
    auto* urgentList = new QListWidget(urgentBox);
    urgentList->setObjectName("actionList");
    urgentList->addItem(QString("%1 survey recovery item(s) are overdue.").arg(overdueRecovery));
    urgentList->addItem(QString("%1 evidence binder item(s) are due now or overdue.").arg(dueBinder));
    urgentList->addItem(QString("%1 mock survey drill item(s) are due now or overdue.").arg(dueDrills));
    urgentList->addItem(QString("%1 entrance-conference item(s) are due now or overdue.").arg(dueEntrance));
    urgentList->addItem(QString("%1 barrier escalation item(s) are urgent.").arg(urgentBarriers));
    urgentList->addItem(QString("%1 executive follow-up item(s) remain blocked.").arg(blockedExec));
    urgentList->addItem(QString("%1 live survey request(s) are due soon and %2 are already overdue.").arg(liveDueSoon).arg(liveOverdue));
    urgentList->addItem(QString("%1 document request(s) are due today and %2 are marked missing or blocked.").arg(documentDueToday).arg(documentMissing));
    urgentList->addItem(QString("%1 packet(s) are due now and %2 are ready to print/export.").arg(exportDue).arg(exportReady));
    urgentList->addItem(QString("%1 alert-center item(s) are open, %2 are critical, and %3 are blocked.").arg(alertOpen).arg(alertCritical).arg(alertBlocked));
    urgentList->addItem("Use this screen as the morning huddle / survey-day coordination board.");
    urgentLayout->addWidget(urgentHint);
    urgentLayout->addWidget(urgentList);

    auto* priorityBox = new QGroupBox("Top survey priorities today", this);
    auto* priorityLayout = new QVBoxLayout(priorityBox);
    auto* priorityHint = new QLabel("Quick operational cues pulled from the active survey boards.", priorityBox);
    priorityHint->setObjectName("panelHint");
    priorityHint->setWordWrap(true);
    auto* priorityTable = new QTableWidget(priorityBox);
    priorityTable->setColumnCount(3);
    priorityTable->setHorizontalHeaderLabels({"Board", "Current priority", "Count"});
    priorityTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    priorityTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    priorityTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    priorityTable->setRowCount(11);

    auto setPriorityRow = [&](int row, const QString& board, const QString& priority, int count) {
        priorityTable->setItem(row, 0, new QTableWidgetItem(board));
        priorityTable->setItem(row, 1, new QTableWidgetItem(priority));
        priorityTable->setItem(row, 2, new QTableWidgetItem(QString::number(count)));
    };

    setPriorityRow(0, "Survey Recovery", "Overdue corrective actions", overdueRecovery);
    setPriorityRow(1, "Evidence Binder", "Evidence due / still collecting", dueBinder);
    setPriorityRow(2, "Mock Survey Drill", "Drills due before walkthrough", dueDrills);
    setPriorityRow(3, "Entrance Conference", "Packets / requests not yet ready", dueEntrance);
    setPriorityRow(4, "Barrier Escalation", "High or critical barriers", urgentBarriers);
    setPriorityRow(5, "Executive Follow-Up", "Blocked closure items", blockedExec);
    setPriorityRow(6, "Document Request Log", "Missing / due-today packets", documentDueToday + documentMissing);
    setPriorityRow(7, "Resident Tracer Manager", "Tracer findings still needing correction", tracerHighRisk + tracerFollowup);
    setPriorityRow(8, "Plan of Correction", "Open and evidence-pending corrective actions", pocOverdue + pocEvidence);
    setPriorityRow(9, "Executive Print & Export", "Packets due or ready to hand off", exportDue + exportReady);
    setPriorityRow(10, "Alerts & Escalation", "Critical, blocked, or due-now urgency items", alertCritical + alertBlocked + alertDue);

    priorityLayout->addWidget(priorityHint);
    priorityLayout->addWidget(priorityTable);

    lowerRow->addWidget(urgentBox, 2);
    lowerRow->addWidget(priorityBox, 3);
    root->addLayout(lowerRow);

    auto* detailBox = new QGroupBox("Board detail summary", this);
    auto* detailLayout = new QVBoxLayout(detailBox);
    auto* detailHint = new QLabel("Each row condenses one of the major survey-readiness workspaces into a single status line for leadership.", detailBox);
    detailHint->setObjectName("panelHint");
    detailHint->setWordWrap(true);

    auto* detailTable = new QTableWidget(detailBox);
    detailTable->setColumnCount(5);
    detailTable->setHorizontalHeaderLabels({"Board", "Open", "Due / blocked", "Ready / complete", "Leadership read"});
    detailTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    detailTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    detailTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    detailTable->setRowCount(11);

    auto setDetailRow = [&](int row, const QString& board, int open, int dueBlocked, int ready, const QString& read) {
        detailTable->setItem(row, 0, new QTableWidgetItem(board));
        detailTable->setItem(row, 1, new QTableWidgetItem(QString::number(open)));
        detailTable->setItem(row, 2, new QTableWidgetItem(QString::number(dueBlocked)));
        detailTable->setItem(row, 3, new QTableWidgetItem(QString::number(ready)));
        detailTable->setItem(row, 4, new QTableWidgetItem(read));
    };

    setDetailRow(0, "Survey Recovery", recoveryOpen, overdueRecovery, db->countWhere("survey_recovery_items", "status='Complete'"), "Use to close corrective-action gaps and assign evidence owners.");
    setDetailRow(1, "Evidence Binder", binderOpen, dueBinder, db->countWhere("evidence_binder_items", "status='Ready' OR readiness='Ready'"), "Shows binder sections still collecting versus ready to hand over.");
    setDetailRow(2, "Mock Survey Drill", drillOpen, dueDrills, db->countWhere("mock_survey_drills", "status='Complete'"), "Tracks tracer walks, interviews, and observation practice.");
    setDetailRow(3, "Entrance Conference", entranceOpen, dueEntrance, db->countWhere("survey_entrance_conference_items", "status='Ready'"), "Confirms packets, lists, room setup, and requested materials.");
    setDetailRow(4, "Barrier Escalation", barrierOpen, urgentBarriers, db->countWhere("barrier_escalations", "status='Removed'"), "Separates true blockers from normal follow-up work.");
    setDetailRow(5, "Executive Follow-Up", execOpen, blockedExec, db->countWhere("executive_followups", "status='Complete'"), "Shows carry-over leadership work that still threatens readiness.");
    setDetailRow(6, "Document Request Log", documentOpen, documentDueToday + documentMissing, db->countWhere("survey_document_requests", "status='Delivered' OR status='Closed'"), "Tracks policy pulls, rosters, logs, and packets requested during active survey response.");
    setDetailRow(7, "Resident Tracer Manager", tracerOpen, tracerHighRisk + tracerFollowup, db->countWhere("resident_tracer_items", "status='Resolved' OR status='Closed'"), "Tracks resident-centered tracers, risk findings, and immediate follow-up across departments.");
    setDetailRow(8, "Plan of Correction", pocOpen, pocOverdue + pocEvidence, db->countWhere("plan_of_correction_items", "status='Complete' OR status='Submitted'"), "Converts findings into owned corrective action with evidence and monitoring plans.");
    setDetailRow(9, "Executive Print & Export", exportOpen, exportDue + exportReady, db->countWhere("executive_export_packets", "status='Exported' OR status='Printed' OR status='Delivered'"), "Queues leadership packets, briefing sheets, and snapshot exports for huddles and survey operations.");
    setDetailRow(10, "Alerts & Escalation", alertOpen, alertCritical + alertBlocked + alertDue, db->countWhere("alerts_escalation_items", "status='Resolved'"), "Aggregates the urgency layer so leadership can see critical, overdue, and blocked items without opening each source board.");

    detailLayout->addWidget(detailHint);
    detailLayout->addWidget(detailTable);
    root->addWidget(detailBox);

    root->addStretch();
}
