#include "DashboardPage.h"
#include "../../data/DatabaseManager.h"
#include "../widgets/KpiCard.h"

#include <QDate>
#include <QTime>
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

    auto* title = new QLabel("Executive home screen", hero);
    title->setObjectName("dashboardTitle");
    auto* subtitle = new QLabel(
        "The executive home screen stays simple, and v94 keeps the executive home screen simple while adding role-based leadership lenses so the same connected operational picture can be interpreted differently without rebuilding the work.",
        hero);
    subtitle->setObjectName("dashboardSubtitle");
    subtitle->setWordWrap(true);

    const int dailyOpsOpen = db->countWhere("leadership_rounds", "status='Open' OR status='In Progress' OR status='Watch'")
        + db->countWhere("executive_followups", "status='Open' OR status='In Progress' OR status='Watch' OR status='Blocked'")
        + db->countWhere("morning_meeting_items", "status='Open' OR status='In Progress' OR status='Blocked'")
        + db->countWhere("department_pulse_items", "status='Open' OR status='Watching' OR status='Blocked'")
        + db->countWhere("barrier_escalations", "status='Open' OR status='Assigned' OR status='Waiting'");
    const int surveyOpen = db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'")
        + db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'")
        + db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'")
        + db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated'")
        + db->countWhere("plan_of_correction_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review'");
    const int residentCareOpen = db->countWhere("admissions", "status!='Admitted' AND status!='Discharged'")
        + db->countWhere("incidents", "status!='Closed'")
        + db->countWhere("mds_items", "status!='Closed' AND status!='Complete'")
        + db->countWhere("social_services_items", "status='Open' OR status='In Progress'")
        + db->countWhere("therapy_items");
    const int supportOpen = db->countWhere("staffing_assignments", "status='Open'")
        + db->countWhere("tasks", "status='Open' OR status='In Progress' OR status='Blocked'")
        + db->countWhere("quality_followups", "status!='Closed' AND status!='Complete'")
        + db->countWhere("document_items", "status!='Closed'");

    auto* snapshot = new QLabel(
        QString("%1 current residents · %2 open daily-ops items · %3 open survey items · %4 open resident-care items · %5 open support items · %6 minimum staffing gaps · %7 overdue alerts · %8 huddles ready or drafting")
            .arg(db->countWhere("residents", "status='Current'"))
            .arg(dailyOpsOpen)
            .arg(surveyOpen)
            .arg(residentCareOpen)
            .arg(supportOpen)
            .arg(db->countMinimumStaffingGaps())
            .arg(db->overdueAlertCount())
            .arg(db->countWhere("leadership_huddle_agendas", "status='Drafting' OR status='Ready'")),
        hero);
    snapshot->setObjectName("dashboardSnapshot");

    const auto prefRows = db->fetchTable("dashboard_preferences", {"pref_key", "pref_value"});
    QString focusNote;
    QString densityMode = "Comfortable";
    QString pinnedModules;
    for (const auto& row : prefRows) {
        if (row.value("pref_key") == "focus_note") focusNote = row.value("pref_value");
        if (row.value("pref_key") == "density_mode") densityMode = row.value("pref_value");
        if (row.value("pref_key") == "pinned_modules") pinnedModules = row.value("pref_value");
    }

    heroLayout->addWidget(title);
    heroLayout->addWidget(subtitle);
    heroLayout->addWidget(snapshot);
    if (!focusNote.trimmed().isEmpty()) {
        auto* focusLabel = new QLabel(QString("Executive focus: %1").arg(focusNote), hero);
        focusLabel->setObjectName("panelHint");
        focusLabel->setWordWrap(true);
        heroLayout->addWidget(focusLabel);
    }
    if (!pinnedModules.trimmed().isEmpty()) {
        auto* pinnedLabel = new QLabel(QString("Pinned modules: %1").arg(pinnedModules), hero);
        pinnedLabel->setObjectName("panelHint");
        pinnedLabel->setWordWrap(true);
        heroLayout->addWidget(pinnedLabel);
    }
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
        v->setObjectName("summaryValue");
        wrap->addWidget(l);
        wrap->addWidget(v);
        summaryLayout->addLayout(wrap);
    };
    addSummary("Ready admits", QString::number(db->countWhere("admissions", "status='Ready'")));
    addSummary("Coverage gaps", QString::number(db->countMinimumStaffingGaps()));
    addSummary("Open incidents", QString::number(db->countWhere("incidents", "status!='Closed'")));
    addSummary("Survey pressure", QString::number(db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'") + db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'") + db->countWhere("plan_of_correction_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review'")));
    addSummary("Barriers + alerts", QString::number(db->countWhere("barrier_escalations", "status='Open' OR status='Assigned' OR status='Waiting'") + db->countWhere("alerts_escalation_items", "status='Open' OR status='Due Today' OR status='Blocked'")));
    addSummary("Open tasks", QString::number(db->countWhere("tasks", "status='Open' OR status='In Progress' OR status='Blocked'")));
    addSummary("Docs needing work", QString::number(db->countWhere("document_items", "status!='Closed'")));
    addSummary("Huddles due / ready", QString::number(db->countWhere("leadership_huddle_agendas", "status='Drafting' OR status='Ready'")));
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
    kpiGrid->addWidget(new KpiCard("Audit log events", QString::number(db->countWhere("audit_log")), this), 8, 0);
    kpiGrid->addWidget(new KpiCard("KPI trend rows", QString::number(db->countWhere("kpi_trend_rows")), this), 8, 1);
    kpiGrid->addWidget(new KpiCard("Off-track KPI rows", QString::number(db->countWhere("kpi_trend_rows", "trend_status='Off Track'")), this), 8, 2);
    kpiGrid->addWidget(new KpiCard("Sync profiles", QString::number(db->countWhere("external_sync_profiles")), this), 9, 0);
    kpiGrid->addWidget(new KpiCard("Ready syncs", QString::number(db->countWhere("external_sync_profiles", "status='Ready'")), this), 9, 1);
    kpiGrid->addWidget(new KpiCard("Planned syncs", QString::number(db->countWhere("external_sync_profiles", "status='Planned'")), this), 9, 2);
    kpiGrid->addWidget(new KpiCard("Release items", QString::number(db->countWhere("release_candidate_items")), this), 10, 0);
    kpiGrid->addWidget(new KpiCard("Ready to deploy", QString::number(db->countWhere("release_candidate_items", "status='Ready' OR status='Complete'")), this), 10, 1);
    kpiGrid->addWidget(new KpiCard("Open rollout items", QString::number(db->countWhere("release_candidate_items", "status='Open' OR status='Watch'")), this), 10, 2);
    kpiGrid->addWidget(new KpiCard("SOP items", QString::number(db->countWhere("sop_items")), this), 11, 0);
    kpiGrid->addWidget(new KpiCard("Active SOPs", QString::number(db->countWhere("sop_items", "status='Active'")), this), 11, 1);
    kpiGrid->addWidget(new KpiCard("SOPs need update", QString::number(db->countWhere("sop_items", "status='Needs Update'")), this), 11, 2);
    kpiGrid->addWidget(new KpiCard("Barrier items", QString::number(db->countWhere("barrier_escalations", "status='Open' OR status='Assigned' OR status='Waiting'")), this), 12, 0);
    kpiGrid->addWidget(new KpiCard("Urgent barriers", QString::number(db->countWhere("barrier_escalations", "severity='High' OR severity='Critical'")), this), 12, 1);
    kpiGrid->addWidget(new KpiCard("Removed barriers", QString::number(db->countWhere("barrier_escalations", "status='Removed'")), this), 12, 2);
    kpiGrid->addWidget(new KpiCard("Recovery items", QString::number(db->countWhere("survey_recovery_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence'")), this), 13, 0);
    kpiGrid->addWidget(new KpiCard("Overdue recovery", QString::number(db->countWhere("survey_recovery_items", QString("(status='Open' OR status='In Progress' OR status='Awaiting Evidence') AND due_date < '%1'").arg(QDate::currentDate().toString("yyyy-MM-dd")))), this), 13, 1);
    kpiGrid->addWidget(new KpiCard("Completed recovery", QString::number(db->countWhere("survey_recovery_items", "status='Complete'")), this), 13, 2);
    kpiGrid->addWidget(new KpiCard("Binder items", QString::number(db->countWhere("evidence_binder_items", "status='Open' OR status='Collecting'")), this), 14, 0);
    kpiGrid->addWidget(new KpiCard("Due binder items", QString::number(db->countWhere("evidence_binder_items", QString("(status='Open' OR status='Collecting') AND due_date <= '%1'").arg(QDate::currentDate().toString("yyyy-MM-dd")))), this), 14, 1);
    kpiGrid->addWidget(new KpiCard("Ready binder items", QString::number(db->countWhere("evidence_binder_items", "status='Ready' OR readiness='Ready'")), this), 14, 2);
    kpiGrid->addWidget(new KpiCard("Mock drill items", QString::number(db->countWhere("mock_survey_drills", "status='Open' OR status='Assigned' OR status='In Drill'")), this), 15, 0);
    kpiGrid->addWidget(new KpiCard("Due mock drills", QString::number(db->countWhere("mock_survey_drills", QString("(status='Open' OR status='Assigned' OR status='In Drill') AND due_date <= '%1'").arg(QDate::currentDate().toString("yyyy-MM-dd")))), this), 15, 1);
    kpiGrid->addWidget(new KpiCard("Completed drills", QString::number(db->countWhere("mock_survey_drills", "status='Complete'")), this), 15, 2);
    kpiGrid->addWidget(new KpiCard("Entrance items", QString::number(db->countWhere("survey_entrance_conference_items", "status='Open' OR status='Assigned' OR status='Assembling'")), this), 16, 0);
    kpiGrid->addWidget(new KpiCard("Due entrance items", QString::number(db->countWhere("survey_entrance_conference_items", QString("(status='Open' OR status='Assigned' OR status='Assembling') AND due_date <= '%1'").arg(QDate::currentDate().toString("yyyy-MM-dd")))), this), 16, 1);
    kpiGrid->addWidget(new KpiCard("Ready packets", QString::number(db->countWhere("survey_entrance_conference_items", "status='Ready'")), this), 16, 2);
    kpiGrid->addWidget(new KpiCard("Survey command items", QString::number(db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'")), this), 17, 0);
    kpiGrid->addWidget(new KpiCard("High-priority command", QString::number(db->countWhere("survey_command_items", "priority='High' AND status!='Closed' AND status!='Complete'")), this), 17, 1);
    kpiGrid->addWidget(new KpiCard("Command complete", QString::number(db->countWhere("survey_command_items", "status='Complete' OR status='Closed'")), this), 17, 2);
    kpiGrid->addWidget(new KpiCard("Live survey requests", QString::number(db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'")), this), 18, 0);
    kpiGrid->addWidget(new KpiCard("Live requests overdue", QString::number(db->countWhere("survey_live_requests", QString("(status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon') AND request_date < '%1'").arg(QDate::currentDate().toString("yyyy-MM-dd"))) + db->countWhere("survey_live_requests", QString("(status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon') AND request_date='%1' AND due_time < '%2'").arg(QDate::currentDate().toString("yyyy-MM-dd"), QTime::currentTime().toString("HH:mm")))), this), 18, 1);
    kpiGrid->addWidget(new KpiCard("Requests delivered", QString::number(db->countWhere("survey_live_requests", "status='Delivered' OR status='Closed'")), this), 18, 2);
    kpiGrid->addWidget(new KpiCard("Survey doc requests", QString::number(db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'")), this), 19, 0);
    kpiGrid->addWidget(new KpiCard("Doc due today", QString::number(db->countWhere("survey_document_requests", QString("(status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing') AND due_date <= '%1'").arg(QDate::currentDate().toString("yyyy-MM-dd")))), this), 19, 1);
    kpiGrid->addWidget(new KpiCard("Doc missing", QString::number(db->countWhere("survey_document_requests", "status='Missing'")), this), 19, 2);
    kpiGrid->addWidget(new KpiCard("Resident tracers", QString::number(db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated'")), this), 20, 0);
    kpiGrid->addWidget(new KpiCard("High-risk tracers", QString::number(db->countWhere("resident_tracer_items", "(status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated') AND (risk_level='High' OR risk_level='Critical')")), this), 20, 1);
    kpiGrid->addWidget(new KpiCard("Tracer follow-up", QString::number(db->countWhere("resident_tracer_items", "status='Needs Follow-Up' OR status='Escalated'")), this), 20, 2);
    kpiGrid->addWidget(new KpiCard("POC open", QString::number(db->countWhere("plan_of_correction_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review'")), this), 21, 0);
    kpiGrid->addWidget(new KpiCard("POC overdue", QString::number(db->countWhere("plan_of_correction_items", QString("(status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review') AND due_date < '%1'").arg(QDate::currentDate().toString("yyyy-MM-dd")))), this), 21, 1);
    kpiGrid->addWidget(new KpiCard("POC awaiting evidence", QString::number(db->countWhere("plan_of_correction_items", "status='Awaiting Evidence' OR status='Under Review'")), this), 21, 2);
    kpiGrid->addWidget(new KpiCard("Exec packets", QString::number(db->countWhere("executive_export_packets", "status='Drafting' OR status='Waiting on Input' OR status='Ready'")), this), 22, 0);
    kpiGrid->addWidget(new KpiCard("Packets due now", QString::number(db->countWhere("executive_export_packets", QString("(status='Drafting' OR status='Waiting on Input' OR status='Ready') AND due_date <= '%1'").arg(QDate::currentDate().toString("yyyy-MM-dd")))), this), 22, 1);
    kpiGrid->addWidget(new KpiCard("Packets exported", QString::number(db->countWhere("executive_export_packets", "status='Exported' OR status='Printed' OR status='Delivered'")), this), 22, 2);
    kpiGrid->addWidget(new KpiCard("Alert-center open", QString::number(db->countWhere("alerts_escalation_items", "status='Open' OR status='Due Today' OR status='Blocked'")), this), 23, 0);
    kpiGrid->addWidget(new KpiCard("Critical alerts", QString::number(db->countWhere("alerts_escalation_items", "(status='Open' OR status='Due Today' OR status='Blocked') AND severity='Critical'")), this), 23, 1);
    kpiGrid->addWidget(new KpiCard("Blocked alerts", QString::number(db->countWhere("alerts_escalation_items", "status='Blocked'")), this), 23, 2);
    kpiGrid->addWidget(new KpiCard("Huddle agendas", QString::number(db->countWhere("leadership_huddle_agendas", "status='Drafting' OR status='Ready'")), this), 24, 0);
    kpiGrid->addWidget(new KpiCard("Huddles due now", QString::number(db->countWhere("leadership_huddle_agendas", QString("(status='Drafting' OR status='Ready') AND (huddle_date < '%1' OR (huddle_date='%1' AND due_time <= '%2'))").arg(QDate::currentDate().toString("yyyy-MM-dd"), QTime::currentTime().toString("HH:mm")))), this), 24, 1);
    kpiGrid->addWidget(new KpiCard("Huddles completed", QString::number(db->countWhere("leadership_huddle_agendas", "status='Completed'")), this), 24, 2);
    root->addLayout(kpiGrid);

    auto* lowerRow = new QHBoxLayout();
    lowerRow->setSpacing(16);

    auto* actionBox = new QGroupBox("Priority queue", this);
    auto* actionLayout = new QVBoxLayout(actionBox);
    auto* actionHint = new QLabel("Highest-value work pulled into one short queue so leadership can decide what needs attention first without scanning every board.", actionBox);
    actionHint->setObjectName("panelHint");
    actionHint->setWordWrap(true);
    auto* list = new QListWidget(actionBox);
    list->setObjectName("actionList");
    for (const auto& item : db->actionCenterItems()) {
        list->addItem(item.first + " — " + item.second);
    }
    actionLayout->addWidget(actionHint);
    actionLayout->addWidget(list);

    auto* quickBox = new QGroupBox("Quick building brief", this);
    auto* quickLayout = new QVBoxLayout(quickBox);
    auto* quickHint = new QLabel(QString("Use this to orient the building quickly before moving into a detailed module. Density mode: %1.").arg(densityMode), quickBox);
    quickHint->setObjectName("panelHint");
    quickHint->setWordWrap(true);
    auto* quickList = new QListWidget(quickBox);
    quickList->setObjectName("actionList");
    quickList->addItem(QString("%1 assignment(s) still need staffing coverage").arg(db->countWhere("staffing_assignments", "status='Open'")));
    quickList->addItem(QString("%1 staffing group(s) are below minimum coverage").arg(db->countMinimumStaffingGaps()));
    quickList->addItem(QString("%1 referral(s) are ready for admit and %2 still need documents").arg(db->countWhere("admissions", "status='Ready'"), db->countWhere("admissions", "status='Needs Docs'")));
    quickList->addItem(QString("%1 incident(s) and %2 quality follow-up item(s) remain open").arg(db->countWhere("incidents", "status!='Closed'"), db->countWhere("quality_followups", "status!='Closed' AND status!='Complete'")));
    quickList->addItem(QString("%1 survey request(s), %2 document pull(s), and %3 POC item(s) are still active").arg(db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'"), db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'"), db->countWhere("plan_of_correction_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review'")));
    quickList->addItem(QString("%1 barrier item(s) and %2 alert-center item(s) still need leadership attention").arg(db->countWhere("barrier_escalations", "status='Open' OR status='Assigned' OR status='Waiting'"), db->countWhere("alerts_escalation_items", "status='Open' OR status='Due Today' OR status='Blocked'")));
    quickList->addItem(QString("%1 resident tracer(s) and %2 evidence-binder item(s) remain active").arg(db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated'"), db->countWhere("evidence_binder_items", "status='Open' OR status='Collecting'")));
    quickList->addItem(QString("%1 document-center item(s) and %2 MDS item(s) still need work").arg(db->countWhere("document_items", "status!='Closed'"), db->countWhere("mds_items", "status!='Closed' AND status!='Complete'")));
    quickList->addItem(QString("%1 huddle agenda(s) are drafting or ready, and %2 export packet(s) are still being prepared").arg(db->countWhere("leadership_huddle_agendas", "status='Drafting' OR status='Ready'"), db->countWhere("executive_export_packets", "status='Drafting' OR status='Waiting on Input' OR status='Ready'")));
    quickList->addItem("Reports workspace can export a daily summary, census CSV, and staffing CSV.");
    quickLayout->addWidget(quickHint);
    quickLayout->addWidget(quickList);

    lowerRow->addWidget(actionBox, 3);
    lowerRow->addWidget(quickBox, 2);
    root->addLayout(lowerRow);
    root->addStretch();
}
