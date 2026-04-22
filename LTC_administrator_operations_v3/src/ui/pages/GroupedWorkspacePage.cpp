#include "GroupedWorkspacePage.h"

#include "../../data/DatabaseManager.h"
#include "../widgets/GlobalInsightPanel.h"

#include <QDateTime>
#include <QFrame>
#include <QLabel>
#include <QShowEvent>
#include <QTabWidget>
#include <QVBoxLayout>

GroupedWorkspacePage::GroupedWorkspacePage(DatabaseManager* db,
                                           const QString& contextKey,
                                           const QString& title,
                                           const QString& subtitle,
                                           const QList<QPair<QString, QWidget*>>& sections,
                                           QWidget* parent)
    : QWidget(parent), m_db(db), m_contextKey(contextKey), m_sections(sections) {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(14);

    auto* hero = new QFrame(this);
    hero->setObjectName("heroPanel");
    auto* heroLayout = new QVBoxLayout(hero);
    heroLayout->setContentsMargins(18, 18, 18, 18);
    heroLayout->setSpacing(8);

    auto* titleLabel = new QLabel(title, hero);
    titleLabel->setObjectName("dashboardTitle");

    auto* subtitleLabel = new QLabel(subtitle, hero);
    subtitleLabel->setObjectName("dashboardSubtitle");
    subtitleLabel->setWordWrap(true);

    heroLayout->addWidget(titleLabel);
    heroLayout->addWidget(subtitleLabel);
    rootLayout->addWidget(hero);

    m_summaryPanel = new GlobalInsightPanel(db, contextKey, this);
    rootLayout->addWidget(m_summaryPanel);

    m_refreshStatusLabel = new QLabel(this);
    m_refreshStatusLabel->setObjectName("panelHint");
    m_refreshStatusLabel->setWordWrap(true);
    rootLayout->addWidget(m_refreshStatusLabel);

    m_tabs = new QTabWidget(this);
    m_tabs->setDocumentMode(true);
    m_tabs->setMovable(false);
    m_tabs->setUsesScrollButtons(true);

    for (const auto& section : m_sections) {
        m_tabs->addTab(section.second, section.first);
    }

    QObject::connect(m_tabs, &QTabWidget::currentChanged, this, [this](int) {
        refreshConnectedState();
    });
    QObject::connect(m_db, &DatabaseManager::dataChanged, this, [this](const QString&) {
        refreshConnectedState();
    });

    rootLayout->addWidget(m_tabs, 1);

    refreshConnectedState();
}

void GroupedWorkspacePage::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    refreshConnectedState();
}

QString GroupedWorkspacePage::tabLabelFor(const QString& originalTitle) const {
    if (!m_db) return originalTitle;

    if (m_contextKey == "daily") {
        if (originalTitle == "Leadership Rounds") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("leadership_rounds", "status='Open' OR status='In Progress'"));
        if (originalTitle == "Executive Follow-Up") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("executive_followups", "status='Open' OR status='Assigned' OR status='Escalated'"));
        if (originalTitle == "Morning Meeting") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("morning_meeting_items", "status='Open' OR status='In Progress' OR status='Escalated'"));
        if (originalTitle == "Department Pulse") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("department_pulse_items", "status='Open' OR status='Watching' OR status='Blocked'"));
        if (originalTitle == "Barrier Escalation") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("barrier_escalations", "status='Open' OR status='In Progress' OR status='Escalated'"));
        if (originalTitle == "Alerts") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("alerts_items", "status='Open' OR status='Monitoring'"));
        if (originalTitle == "Leadership Huddle") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("leadership_huddle_agendas", "status='Draft' OR status='Ready' OR status='In Progress'"));
    } else if (m_contextKey == "survey") {
        if (originalTitle == "Command Center") return QString("%1 (%2)").arg(originalTitle).arg(
            m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='In Progress' OR status='Due Soon'") +
            m_db->countWhere("survey_document_requests", "status='Open' OR status='In Progress' OR status='Missing' OR status='Blocked'") +
            m_db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='At Risk'"));
        if (originalTitle == "Survey Recovery") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("survey_recovery_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence'"));
        if (originalTitle == "Evidence Binder") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("evidence_binder_items", "status='Open' OR status='Collecting'"));
        if (originalTitle == "Mock Drill") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("mock_survey_drills", "status='Open' OR status='Assigned' OR status='In Drill'"));
        if (originalTitle == "Entrance Conference") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("survey_entrance_conference_items", "status='Open' OR status='In Progress' OR status='Pending'"));
        if (originalTitle == "Alerts & Escalation") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("alerts_escalation_items", "status='Open' OR status='Watching' OR status='Blocked'"));
        if (originalTitle == "Live Response") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='In Progress' OR status='Due Soon'"));
        if (originalTitle == "Document Requests") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("survey_document_requests", "status='Open' OR status='In Progress' OR status='Missing' OR status='Blocked'"));
        if (originalTitle == "Resident Tracers") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='At Risk'"));
        if (originalTitle == "Plan of Correction") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("plan_of_correction_items", "status='Open' OR status='Drafting' OR status='Awaiting Evidence' OR status='Under Review'"));
        if (originalTitle == "Print & Export") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("executive_export_packets", "status='Draft' OR status='Open' OR status='Queued'"));
        if (originalTitle == "Survey Ready") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("survey_items", "status='Open' OR status='At Risk' OR status='In Progress'"));
    } else if (m_contextKey == "resident") {
        if (originalTitle == "Residents") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("residents", "status='Active'"));
        if (originalTitle == "Admissions") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("admissions", "status='Pending' OR status='Ready' OR status='In Progress'"));
        if (originalTitle == "Treatments") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("wound_treatments", "status='Open' OR status='In Progress'"));
        if (originalTitle == "Medical Records") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("pharmacy_items", "status='Open' OR status='Pending' OR status='In Progress'"));
        if (originalTitle == "MDS") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("mds_items", "status='Open' OR status='Due Soon' OR status='In Progress'"));
        if (originalTitle == "DON / Incidents") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("incidents", "status='Open' OR status='Investigating' OR status='Follow-Up'"));
        if (originalTitle == "Social Services") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("social_services_items", "status='Open' OR status='In Progress'"));
        if (originalTitle == "Dietary") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("dietary_items", "status='Open' OR status='In Progress'"));
        if (originalTitle == "Transportation") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("transport_items", "status='Open' OR status='Scheduled' OR status='In Progress'"));
    } else if (m_contextKey == "ops") {
        if (originalTitle == "HR / Staffing") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("staffing_assignments", "status='Open' OR status='Pending' OR status='Call Off' OR status='Needs Coverage'"));
        if (originalTitle == "Tasks") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("tasks", "status='Open' OR status='In Progress' OR status='Blocked'"));
        if (originalTitle == "Huddle") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("huddle_items", "status='Open' OR status='In Progress'"));
        if (originalTitle == "Environmental Services") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("environmental_rounds", "status='Open' OR status='In Progress'"));
        if (originalTitle == "QAPI") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("pips", "status='Open' OR status='In Progress' OR status='Monitoring'"));
        if (originalTitle == "Quality") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("quality_followups", "status='Open' OR status='Monitoring' OR status='In Progress'"));
        if (originalTitle == "Budget") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("budget_items", "status='Open' OR status='In Review' OR status='Pending'"));
    } else if (m_contextKey == "docs") {
        if (originalTitle == "Document Center") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("document_items", "status='Open' OR status='Draft' OR status='In Review'"));
        if (originalTitle == "Reports") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("quality_monthly_snapshots"));
        if (originalTitle == "Dashboard Setup") return QString("%1 (%2)").arg(originalTitle).arg(m_db->countWhere("dashboard_preferences"));
    }

    return originalTitle;
}

void GroupedWorkspacePage::refreshConnectedState() {
    if (m_summaryPanel) m_summaryPanel->refreshData();
    if (!m_tabs) return;

    for (int i = 0; i < m_sections.size() && i < m_tabs->count(); ++i) {
        m_tabs->setTabText(i, tabLabelFor(m_sections[i].first));
    }

    if (m_refreshStatusLabel) {
        const QString activeTab = (m_tabs->currentIndex() >= 0) ? m_tabs->tabText(m_tabs->currentIndex()) : QString("Overview");
        m_refreshStatusLabel->setText(
            QString("Live global refresh is active. Shared counts and hub insight cards refreshed at %1 while viewing %2.")
                .arg(QDateTime::currentDateTime().toString("MMM d, yyyy h:mm:ss AP"), activeTab));
    }
}
