#include "RoleBasedExecutiveViewsPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {
void setCell(QTableWidget* table, int row, int col, const QString& text) {
    auto* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table->setItem(row, col, item);
}

QString statusClause(const QStringList& statuses) {
    QStringList parts;
    for (const QString& status : statuses) {
        parts << QString("status='%1'").arg(status);
    }
    return parts.join(" OR ");
}

QString dueClause() {
    return "due_date<=date('now') OR due_time<=time('now')";
}
}

RoleBasedExecutiveViewsPage::RoleBasedExecutiveViewsPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Role-Based Executive Views", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v94 adds focused executive perspectives so the same connected operational system can be viewed through administrator, DON, department, or survey-response priorities without removing any existing functionality.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");

    auto* selectorRow = new QHBoxLayout();
    auto* selectorLabel = new QLabel("Executive lens", this);
    selectorLabel->setStyleSheet("font-weight: 600;");
    m_roleCombo = new QComboBox(this);
    m_roleCombo->addItem("Administrator", "administrator");
    m_roleCombo->addItem("DON / Clinical", "don");
    m_roleCombo->addItem("Department Leadership", "department");
    m_roleCombo->addItem("Survey Response Lead", "survey");

    selectorRow->addWidget(selectorLabel);
    selectorRow->addWidget(m_roleCombo, 0);
    selectorRow->addStretch(1);

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setWordWrap(true);
    m_summaryLabel->setStyleSheet(
        "background:#eef4f8; border:1px solid #d9e2ec; border-radius:10px;"
        "padding:8px 14px; color:#334e68; font-weight:600;");

    auto makeStatCard = [&](const QString& title, QLabel*& valueLabel, const QString& hintText) {
        auto* card = new QGroupBox(title, this);
        auto* layout = new QVBoxLayout(card);
        valueLabel = new QLabel("0", card);
        valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #12344d;");
        auto* hint = new QLabel(hintText, card);
        hint->setWordWrap(true);
        hint->setStyleSheet("color:#5b6472;");
        layout->addWidget(valueLabel);
        layout->addWidget(hint);
        return card;
    };

    auto* statsRow = new QHBoxLayout();
    statsRow->addWidget(makeStatCard("Primary pressure", m_primaryCountLabel, "Highest priority count for the active leadership lens."), 1);
    statsRow->addWidget(makeStatCard("Secondary pressure", m_secondaryCountLabel, "The next queue that the selected role usually needs to touch."), 1);
    statsRow->addWidget(makeStatCard("Due now", m_tertiaryCountLabel, "Items due now or already overdue for the active role."), 1);

    auto* bodyRow = new QHBoxLayout();

    auto* leftBox = new QGroupBox("Priority queue", this);
    auto* leftLayout = new QVBoxLayout(leftBox);
    auto* leftHint = new QLabel("Top action buckets for the selected leadership lens.", leftBox);
    leftHint->setWordWrap(true);
    leftHint->setStyleSheet("color:#5b6472;");
    m_priorityList = new QListWidget(leftBox);
    leftLayout->addWidget(leftHint);
    leftLayout->addWidget(m_priorityList);

    auto* centerBox = new QGroupBox("Role view summary", this);
    auto* centerLayout = new QVBoxLayout(centerBox);
    auto* centerHint = new QLabel("How each connected board should be interpreted by the selected role.", centerBox);
    centerHint->setWordWrap(true);
    centerHint->setStyleSheet("color:#5b6472;");
    m_boardSummaryTable = new QTableWidget(0, 3, centerBox);
    m_boardSummaryTable->setHorizontalHeaderLabels({"Board", "Current pressure", "Why it matters"});
    m_boardSummaryTable->horizontalHeader()->setStretchLastSection(true);
    m_boardSummaryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_boardSummaryTable->verticalHeader()->setVisible(false);
    m_boardSummaryTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_boardSummaryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    centerLayout->addWidget(centerHint);
    centerLayout->addWidget(m_boardSummaryTable);

    auto* rightBox = new QGroupBox("Focused follow-up", this);
    auto* rightLayout = new QVBoxLayout(rightBox);
    auto* rightHint = new QLabel("The connected tabs or topics that should be checked next for this role.", rightBox);
    rightHint->setWordWrap(true);
    rightHint->setStyleSheet("color:#5b6472;");
    m_focusList = new QListWidget(rightBox);
    rightLayout->addWidget(rightHint);
    rightLayout->addWidget(m_focusList);

    bodyRow->addWidget(leftBox, 1);
    bodyRow->addWidget(centerBox, 2);
    bodyRow->addWidget(rightBox, 1);

    root->addWidget(heading);
    root->addWidget(subheading);
    root->addLayout(selectorRow);
    root->addWidget(m_summaryLabel);
    root->addLayout(statsRow);
    root->addLayout(bodyRow, 1);

    QObject::connect(m_roleCombo, &QComboBox::currentIndexChanged, this, [this](int) { refreshView(); });
    QObject::connect(m_db, &DatabaseManager::dataChanged, this, [this](const QString&) { refreshView(); });

    refreshView();
}

void RoleBasedExecutiveViewsPage::refreshView() {
    const QString roleKey = m_roleCombo->currentData().toString();

    int primary = 0;
    int secondary = 0;
    int dueNow = 0;
    QString summary;

    if (roleKey == "administrator") {
        primary = m_db->countWhere("unified_action_items", "status='Open' OR status='In Progress' OR status='Blocked'");
        secondary = m_db->countWhere("alerts_escalation_items", "severity='Critical' OR status='Blocked'");
        dueNow = m_db->countWhere("executive_followups", "status='Open' AND (due_date<=date('now') OR priority='High')");
        summary = "Administrator view emphasizes executive priorities, blocked items, and cross-building follow-up so the home screen, action center, and huddle tools can be interpreted through a command-level lens.";
    } else if (roleKey == "don") {
        primary = m_db->countWhere("plan_of_correction_items", "status='Open' OR status='Awaiting Evidence' OR status='Under Review'");
        secondary = m_db->countWhere("resident_tracer_items", "risk_level='High' OR status='Open'");
        dueNow = m_db->countWhere("survey_live_requests", "status='Open' AND (priority='Urgent' OR due_time<=time('now'))");
        summary = "DON / Clinical view highlights resident tracers, correction plans, live survey requests, and high-risk resident-care follow-up so clinical leadership can see the same system without losing the executive context.";
    } else if (roleKey == "department") {
        primary = m_db->countWhere("department_pulse_items", "status='Open' OR status='Blocked' OR risk_level='High'");
        secondary = m_db->countWhere("morning_meeting_items", "status='Open' OR priority='High'");
        dueNow = m_db->countWhere("barrier_escalations", "status='Open' AND (severity='Urgent' OR due_date<=date('now'))");
        summary = "Department leadership view keeps the focus on operational pulse, morning priorities, and barrier removal so leaders can move from issue visibility to department-level execution faster.";
    } else {
        primary = m_db->countWhere("survey_live_requests", "status='Open' OR priority='Urgent'");
        secondary = m_db->countWhere("survey_document_requests", "status='Open' OR status='Missing' OR status='Blocked'");
        dueNow = m_db->countWhere("survey_recovery_items", "status='Open' AND (priority='High' OR due_date<=date('now'))");
        summary = "Survey response view concentrates on live requests, document pulls, correction items, and survey readiness pressure so the same connected workspace becomes a true survey-day operating view.";
    }

    m_summaryLabel->setText(summary);
    m_primaryCountLabel->setText(QString::number(primary));
    m_secondaryCountLabel->setText(QString::number(secondary));
    m_tertiaryCountLabel->setText(QString::number(dueNow));

    populatePriorityList(roleKey);
    populateBoardSummary(roleKey);
    populateFocusList(roleKey);
}

void RoleBasedExecutiveViewsPage::populatePriorityList(const QString& roleKey) {
    m_priorityList->clear();
    if (roleKey == "administrator") {
        m_priorityList->addItem(QString("Open executive actions: %1").arg(m_db->countWhere("unified_action_items", "status='Open' OR status='In Progress'")));
        m_priorityList->addItem(QString("Critical alerts / escalations: %1").arg(m_db->countWhere("alerts_escalation_items", "severity='Critical' OR status='Blocked'")));
        m_priorityList->addItem(QString("Open executive follow-up: %1").arg(m_db->countWhere("executive_followups", "status='Open'")));
        m_priorityList->addItem(QString("Leadership huddles not completed: %1").arg(m_db->countWhere("leadership_huddle_agendas", "status!='Completed'")));
    } else if (roleKey == "don") {
        m_priorityList->addItem(QString("Open correction-plan items: %1").arg(m_db->countWhere("plan_of_correction_items", "status='Open' OR status='Awaiting Evidence' OR status='Under Review'")));
        m_priorityList->addItem(QString("High-risk resident tracers: %1").arg(m_db->countWhere("resident_tracer_items", "risk_level='High'")));
        m_priorityList->addItem(QString("Clinical live survey requests: %1").arg(m_db->countWhere("survey_live_requests", "status='Open'")));
        m_priorityList->addItem(QString("Resident-care incidents still open: %1").arg(m_db->countWhere("incidents", "status='Open' OR status='Investigating'")));
    } else if (roleKey == "department") {
        m_priorityList->addItem(QString("Open department pulse issues: %1").arg(m_db->countWhere("department_pulse_items", "status='Open' OR status='Blocked'")));
        m_priorityList->addItem(QString("Morning meeting items still active: %1").arg(m_db->countWhere("morning_meeting_items", "status='Open' OR status='In Progress'")));
        m_priorityList->addItem(QString("Barriers needing removal: %1").arg(m_db->countWhere("barrier_escalations", "status='Open'")));
        m_priorityList->addItem(QString("Open task-board items: %1").arg(m_db->countWhere("tasks", "status='Open' OR status='In Progress'")));
    } else {
        m_priorityList->addItem(QString("Open live survey requests: %1").arg(m_db->countWhere("survey_live_requests", "status='Open'")));
        m_priorityList->addItem(QString("Open or missing document requests: %1").arg(m_db->countWhere("survey_document_requests", "status='Open' OR status='Missing' OR status='Blocked'")));
        m_priorityList->addItem(QString("Open survey recovery items: %1").arg(m_db->countWhere("survey_recovery_items", "status='Open'")));
        m_priorityList->addItem(QString("Open evidence binder items: %1").arg(m_db->countWhere("evidence_binder_items", "status='Open' OR readiness_level!='Ready'")));
    }
}

void RoleBasedExecutiveViewsPage::populateBoardSummary(const QString& roleKey) {
    struct SummaryRow { QString board; int pressure; QString why; };
    QList<SummaryRow> rows;
    if (roleKey == "administrator") {
        rows = {
            {"Executive home", m_db->countWhere("unified_action_items", "status='Open' OR status='In Progress'"), "Best first-stop screen for top executive pressure."},
            {"Unified Action Center", m_db->countWhere("unified_action_items", "status='Open' OR status='Blocked'"), "Cross-building queue that still needs direct leadership movement."},
            {"Daily Ops Hub", m_db->countWhere("executive_followups", "status='Open'") + m_db->countWhere("barrier_escalations", "status='Open'"), "Shows whether rounds, follow-up, and barrier removal are actually closing."},
            {"Reports & print", m_db->countWhere("executive_export_packets", "status='Open' OR status='Ready'"), "Signals handoff and communication readiness for leadership."}
        };
    } else if (roleKey == "don") {
        rows = {
            {"Resident Tracers", m_db->countWhere("resident_tracer_items", "status='Open' OR risk_level='High'"), "Resident-centered survey risk and care-trace pressure."},
            {"Plan of Correction", m_db->countWhere("plan_of_correction_items", "status='Open' OR status='Awaiting Evidence'"), "Clinical correction planning and evidence follow-through."},
            {"Live Survey Response", m_db->countWhere("survey_live_requests", "status='Open'"), "Real-time survey asks that often need clinical leadership response."},
            {"Resident Care Hub", m_db->countWhere("incidents", "status='Open' OR status='Investigating'") + m_db->countWhere("residents", "status='Admitted'"), "Resident status and clinical oversight context."}
        };
    } else if (roleKey == "department") {
        rows = {
            {"Department Pulse", m_db->countWhere("department_pulse_items", "status='Open' OR status='Blocked'"), "Fastest signal for department instability or unclosed problems."},
            {"Morning Meeting", m_db->countWhere("morning_meeting_items", "status='Open' OR status='In Progress'"), "Today’s department execution list."},
            {"Barrier Escalation", m_db->countWhere("barrier_escalations", "status='Open'"), "Items that need cross-department removal or leadership help."},
            {"Operations Support Hub", m_db->countWhere("tasks", "status='Open' OR status='In Progress'") + m_db->countWhere("staff_assignments", "status='Call Off'"), "Shows staffing and support workload that can stall execution."}
        };
    } else {
        rows = {
            {"Survey Command Center", m_db->countWhere("survey_live_requests", "status='Open'") + m_db->countWhere("survey_document_requests", "status='Open' OR status='Missing'"), "The most concentrated survey-day operational view."},
            {"Document Requests", m_db->countWhere("survey_document_requests", "status='Open' OR status='Missing' OR status='Blocked'"), "Tracks missing survey pulls and delayed evidence."},
            {"Survey Recovery", m_db->countWhere("survey_recovery_items", "status='Open'"), "Corrective items that still threaten readiness."},
            {"Print & Export", m_db->countWhere("executive_export_packets", "status='Open' OR status='Ready'"), "Packet readiness for survey-day handoff."}
        };
    }

    m_boardSummaryTable->setRowCount(rows.size());
    for (int row = 0; row < rows.size(); ++row) {
        setCell(m_boardSummaryTable, row, 0, rows[row].board);
        setCell(m_boardSummaryTable, row, 1, QString::number(rows[row].pressure));
        setCell(m_boardSummaryTable, row, 2, rows[row].why);
    }
}

void RoleBasedExecutiveViewsPage::populateFocusList(const QString& roleKey) {
    m_focusList->clear();
    if (roleKey == "administrator") {
        m_focusList->addItem("Open Unified Action Center after the home screen to clear blocked executive items.");
        m_focusList->addItem("Check Shared Notes & Follow-Up for handoff threads before the leadership huddle.");
        m_focusList->addItem("Use Reports & Print when the leadership team needs a current building handoff packet.");
    } else if (roleKey == "don") {
        m_focusList->addItem("Review Resident Tracers before opening Plan of Correction so the clinical story stays connected.");
        m_focusList->addItem("Use Shared Notes & Follow-Up to keep survey-day clinical handoffs visible across tabs.");
        m_focusList->addItem("Check Live Survey Response whenever the command center shows new or overdue requests.");
    } else if (roleKey == "department") {
        m_focusList->addItem("Start in Morning Meeting, then confirm barriers and staffing issues in the connected hubs.");
        m_focusList->addItem("Use Department Pulse for unresolved risk, then Unified Action Center if escalation is needed.");
        m_focusList->addItem("Return to Leadership Huddle when department issues need cross-building visibility.");
    } else {
        m_focusList->addItem("Stay between Survey Command Center, Live Response, and Document Requests during active survey pressure.");
        m_focusList->addItem("Use Plan of Correction and Evidence Binder when the issue shifts from response to closure.");
        m_focusList->addItem("Use Print & Export to prepare handoff packets without rebuilding the survey picture manually.");
    }
}
