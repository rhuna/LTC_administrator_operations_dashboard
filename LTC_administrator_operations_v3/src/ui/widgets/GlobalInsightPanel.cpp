#include "GlobalInsightPanel.h"

#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

GlobalInsightPanel::GlobalInsightPanel(DatabaseManager* db,
                                       const QString& contextKey,
                                       QWidget* parent)
    : QWidget(parent), m_db(db), m_contextKey(contextKey) {
    buildUi();
    refreshData();
}

void GlobalInsightPanel::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(12);

    auto* headingBox = new QGroupBox(this);
    headingBox->setObjectName("heroPanel");
    auto* headingLayout = new QVBoxLayout(headingBox);
    headingLayout->setContentsMargins(16, 14, 16, 14);
    headingLayout->setSpacing(6);

    m_headingLabel = new QLabel(headingBox);
    m_headingLabel->setObjectName("dashboardTitle");
    m_headingLabel->setStyleSheet("font-size:18px;");

    m_summaryLabel = new QLabel(headingBox);
    m_summaryLabel->setObjectName("dashboardSubtitle");
    m_summaryLabel->setWordWrap(true);

    headingLayout->addWidget(m_headingLabel);
    headingLayout->addWidget(m_summaryLabel);
    root->addWidget(headingBox);

    auto* cardGrid = new QGridLayout();
    cardGrid->setHorizontalSpacing(12);
    cardGrid->setVerticalSpacing(12);

    for (int i = 0; i < 4; ++i) {
        auto* card = new QGroupBox(this);
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(14, 12, 14, 12);
        cardLayout->setSpacing(4);

        auto* value = new QLabel("0", card);
        value->setStyleSheet("font-size:24px; font-weight:700; color:#102a43;");
        auto* hint = new QLabel(card);
        hint->setWordWrap(true);
        hint->setStyleSheet("color:#5b6472;");

        cardLayout->addWidget(value);
        cardLayout->addWidget(hint);
        cardGrid->addWidget(card, i / 2, i % 2);
        m_cards.append(qMakePair(value, hint));
    }

    root->addLayout(cardGrid);

    auto* highlightsBox = new QGroupBox("Connected workflow highlights", this);
    auto* highlightsLayout = new QVBoxLayout(highlightsBox);
    highlightsLayout->setContentsMargins(14, 12, 14, 12);

    m_highlightsList = new QListWidget(highlightsBox);
    m_highlightsList->setObjectName("compactInsightList");
    m_highlightsList->setAlternatingRowColors(false);
    m_highlightsList->setSelectionMode(QAbstractItemView::NoSelection);
    m_highlightsList->setFocusPolicy(Qt::NoFocus);
    m_highlightsList->setMaximumHeight(170);

    highlightsLayout->addWidget(m_highlightsList);

    root->addWidget(highlightsBox);

    auto* sharedBox = new QGroupBox("Cross-page shared records", this);
    auto* sharedLayout = new QVBoxLayout(sharedBox);
    sharedLayout->setContentsMargins(14, 12, 14, 12);

    m_sharedLinksList = new QListWidget(sharedBox);
    m_sharedLinksList->setObjectName("compactInsightList");
    m_sharedLinksList->setAlternatingRowColors(false);
    m_sharedLinksList->setSelectionMode(QAbstractItemView::NoSelection);
    m_sharedLinksList->setFocusPolicy(Qt::NoFocus);
    sharedLayout->addWidget(m_sharedLinksList);
    root->addWidget(sharedBox);
}

void GlobalInsightPanel::setCardText(QLabel* valueLabel,
                                     QLabel* hintLabel,
                                     const QString& value,
                                     const QString& hint) {
    if (valueLabel) valueLabel->setText(value);
    if (hintLabel) hintLabel->setText(hint);
}

void GlobalInsightPanel::rebuildHighlights(const QList<QPair<QString, QString>>& highlights) {
    m_highlightsList->clear();
    for (const auto& item : highlights) {
        m_highlightsList->addItem(QString("%1 — %2").arg(item.first, item.second));
    }
}

void GlobalInsightPanel::refreshData() {
    if (!m_db) return;

    QList<QPair<QString, QString>> highlights;

    if (m_contextKey == "daily") {
        const int roundsOpen = m_db->countWhere("leadership_rounds", "status='Open' OR status='In Progress'");
        const int followupsOpen = m_db->countWhere("executive_followups", "status='Open' OR status='Assigned' OR status='Escalated'");
        const int barriersOpen = m_db->countWhere("barrier_escalations", "status='Open' OR status='In Progress' OR status='Escalated'");
        const int alertsOpen = m_db->countWhere("alerts_items", "status='Open' OR status='Monitoring'");

        m_headingLabel->setText("Global Daily Operations Pulse");
        m_summaryLabel->setText("This hub now surfaces shared daily state so rounds, follow-up, barriers, alerts, and huddle prep reflect one another instead of behaving like separate tabs.");
        setCardText(m_cards[0].first, m_cards[0].second, QString::number(roundsOpen), "Open leadership rounds feeding follow-up and meeting pressure.");
        setCardText(m_cards[1].first, m_cards[1].second, QString::number(followupsOpen), "Executive follow-ups still open across the building.");
        setCardText(m_cards[2].first, m_cards[2].second, QString::number(barriersOpen), "Active barriers that may affect pulse, huddle, or survey work.");
        setCardText(m_cards[3].first, m_cards[3].second, QString::number(alertsOpen), "Daily alerts still open or actively monitored.");

        highlights = {
            {"Rounds → Follow-Up", QString("%1 round items and %2 executive follow-ups are still open.").arg(roundsOpen).arg(followupsOpen)},
            {"Barriers → Huddle", QString("%1 barriers should appear in today’s huddle priorities.").arg(barriersOpen)},
            {"Alerts → Meeting Flow", QString("%1 alert items may need same-day leadership attention.").arg(alertsOpen)},
            {"Morning Alignment", QString("%1 morning-meeting items should align with pulse and follow-up work.")
                .arg(m_db->countWhere("morning_meeting_items", "status='Open' OR status='In Progress' OR status='Escalated'"))}
        };
    } else if (m_contextKey == "survey") {
        const int liveRequests = m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='In Progress' OR status='Due Soon'");
        const int documentRequests = m_db->countWhere("survey_document_requests", "status='Open' OR status='In Progress' OR status='Missing' OR status='Blocked'");
        const int tracers = m_db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='At Risk'");
        const int poc = m_db->countWhere("plan_of_correction_items", "status='Open' OR status='Drafting' OR status='Awaiting Evidence' OR status='Under Review'");

        m_headingLabel->setText("Global Survey Workflow Pulse");
        m_summaryLabel->setText("Every survey tab now rolls into one connected view. Requests, documents, tracers, correction work, export packets, and alert pressure are reflected together inside this workspace.");
        setCardText(m_cards[0].first, m_cards[0].second, QString::number(liveRequests), "Live survey requests still active right now.");
        setCardText(m_cards[1].first, m_cards[1].second, QString::number(documentRequests), "Document pulls still open, missing, blocked, or in progress.");
        setCardText(m_cards[2].first, m_cards[2].second, QString::number(tracers), "Resident tracers still open or showing risk.");
        setCardText(m_cards[3].first, m_cards[3].second, QString::number(poc), "Plan-of-correction items still moving toward closure.");

        highlights = {
            {"Command Center", QString("%1 live requests, %2 document pulls, %3 tracers, and %4 POC items are connected to survey response.")
                .arg(liveRequests).arg(documentRequests).arg(tracers).arg(poc)},
            {"Evidence Pressure", QString("%1 binder items and %2 recovery items are still open.")
                .arg(m_db->countWhere("evidence_binder_items", "status='Open' OR status='Collecting'"))
                .arg(m_db->countWhere("survey_recovery_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence'"))},
            {"Escalation Pressure", QString("%1 alert-center items and %2 barriers may block survey closure.")
                .arg(m_db->countWhere("alerts_escalation_items", "status='Open' OR status='Watching' OR status='Blocked'"))
                .arg(m_db->countWhere("barrier_escalations", "status='Open' OR status='In Progress' OR status='Escalated'"))},
            {"Packet Readiness", QString("%1 packets are still open or in draft before export.")
                .arg(m_db->countWhere("executive_export_packets", "status='Draft' OR status='Open' OR status='Queued'"))}
        };
    } else if (m_contextKey == "resident") {
        const int residents = m_db->countWhere("residents", "status='Active'");
        const int admissions = m_db->countWhere("admissions", "status='Pending' OR status='Ready' OR status='In Progress'");
        const int incidents = m_db->countWhere("incidents", "status='Open' OR status='Investigating' OR status='Follow-Up'");
        const int socials = m_db->countWhere("social_services_items", "status='Open' OR status='In Progress'");

        m_headingLabel->setText("Global Resident Care Pulse");
        m_summaryLabel->setText("Resident care pages are tied together through the same operational picture so admissions, incidents, documentation, services, and transport reflect shared building pressure.");
        setCardText(m_cards[0].first, m_cards[0].second, QString::number(residents), "Active residents in the building.");
        setCardText(m_cards[1].first, m_cards[1].second, QString::number(admissions), "Admissions still pending, ready, or moving in.");
        setCardText(m_cards[2].first, m_cards[2].second, QString::number(incidents), "Incident work still open or under follow-up.");
        setCardText(m_cards[3].first, m_cards[3].second, QString::number(socials), "Social-service items still open or in progress.");

        highlights = {
            {"Admissions → Residents", QString("%1 active residents and %2 active admissions should line up with room, transport, and service load.").arg(residents).arg(admissions)},
            {"Clinical Follow-Up", QString("%1 treatment items and %2 MDS items remain open.")
                .arg(m_db->countWhere("wound_treatments", "status='Open' OR status='In Progress'"))
                .arg(m_db->countWhere("mds_items", "status='Open' OR status='Due Soon' OR status='In Progress'"))},
            {"Service Support", QString("%1 dietary items and %2 transport items are still open.")
                .arg(m_db->countWhere("dietary_items", "status='Open' OR status='In Progress'"))
                .arg(m_db->countWhere("transport_items", "status='Open' OR status='Scheduled' OR status='In Progress'"))},
            {"Risk Visibility", QString("%1 pharmacy items and %2 incident items still need attention.")
                .arg(m_db->countWhere("pharmacy_items", "status='Open' OR status='Pending' OR status='In Progress'"))
                .arg(incidents)}
        };
    } else if (m_contextKey == "ops") {
        const int staffing = m_db->countWhere("staffing_assignments", "status='Open' OR status='Pending' OR status='Call Off' OR status='Needs Coverage'");
        const int tasks = m_db->countWhere("tasks", "status='Open' OR status='In Progress' OR status='Blocked'");
        const int qapi = m_db->countWhere("pips", "status='Open' OR status='In Progress' OR status='Monitoring'");
        const int environmental = m_db->countWhere("environmental_rounds", "status='Open' OR status='In Progress'");

        m_headingLabel->setText("Global Operations Support Pulse");
        m_summaryLabel->setText("Support workflows now show shared building pressure so staffing gaps, task follow-through, QAPI, environmental rounds, and budget work can be read together.");
        setCardText(m_cards[0].first, m_cards[0].second, QString::number(staffing), "Assignments still open, uncovered, or needing support.");
        setCardText(m_cards[1].first, m_cards[1].second, QString::number(tasks), "Task items still open, active, or blocked.");
        setCardText(m_cards[2].first, m_cards[2].second, QString::number(qapi), "Open PIP/QAPI work still in motion.");
        setCardText(m_cards[3].first, m_cards[3].second, QString::number(environmental), "Environmental items still open or in progress.");

        highlights = {
            {"Staffing → Tasks", QString("%1 staffing issues and %2 task items may affect same-day throughput.").arg(staffing).arg(tasks)},
            {"QAPI → Quality", QString("%1 PIP items and %2 quality follow-ups remain open.")
                .arg(qapi).arg(m_db->countWhere("quality_followups", "status='Open' OR status='Monitoring' OR status='In Progress'"))},
            {"Environmental Readiness", QString("%1 environmental rounds and %2 huddle items remain active.")
                .arg(environmental)
                .arg(m_db->countWhere("huddle_items", "status='Open' OR status='In Progress'"))},
            {"Budget View", QString("%1 budget items are still open or under review.")
                .arg(m_db->countWhere("budget_items", "status='Open' OR status='In Review' OR status='Pending'"))}
        };
    } else {
        const int documents = m_db->countWhere("document_items", "status='Open' OR status='Draft' OR status='In Review'");
        const int reports = m_db->countWhere("quality_monthly_snapshots");
        const int dashboardPrefs = m_db->countWhere("dashboard_preferences");
        const int audits = m_db->countWhere("audit_log");

        m_headingLabel->setText("Global Admin Support Pulse");
        m_summaryLabel->setText("Reference, reporting, and setup pages now share one quick overview so documentation, reports, and dashboard configuration feel connected to the live app state.");
        setCardText(m_cards[0].first, m_cards[0].second, QString::number(documents), "Documents still open, drafted, or under review.");
        setCardText(m_cards[1].first, m_cards[1].second, QString::number(reports), "Stored monthly quality snapshots.");
        setCardText(m_cards[2].first, m_cards[2].second, QString::number(dashboardPrefs), "Saved dashboard preference profiles.");
        setCardText(m_cards[3].first, m_cards[3].second, QString::number(audits), "Audit-log rows currently stored.");

        highlights = {
            {"Document Flow", QString("%1 document-center items are still being prepared or reviewed.").arg(documents)},
            {"Reporting View", QString("%1 stored quality snapshots feed reporting context.").arg(reports)},
            {"Setup State", QString("%1 dashboard preference rows are available.").arg(dashboardPrefs)},
            {"Audit Context", QString("%1 audit-log entries capture system activity history.").arg(audits)}
        };
    }

    rebuildHighlights(highlights);
    if (m_sharedLinksList) {
        m_sharedLinksList->clear();
        const auto linked = m_db->sharedRecordHighlights(m_contextKey);
        for (const auto& item : linked) {
            m_sharedLinksList->addItem(QString("%1 — %2").arg(item.first, item.second));
        }
        if (linked.isEmpty()) {
            m_sharedLinksList->addItem("No shared-record bridges are configured for this workspace yet.");
        }
    }
}
