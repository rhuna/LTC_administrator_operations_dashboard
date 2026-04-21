#include "DepartmentDashboardsPage.h"

#include "../../data/DatabaseManager.h"
#include "../widgets/KpiCard.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
int countByValue(const QList<QMap<QString, QString>>& rows, const QString& key, const QString& value) {
    int total = 0;
    for (const auto& row : rows) {
        if (row.value(key).compare(value, Qt::CaseInsensitive) == 0) ++total;
    }
    return total;
}

int countByValues(const QList<QMap<QString, QString>>& rows, const QString& key, const QStringList& values) {
    int total = 0;
    for (const auto& row : rows) {
        const QString cell = row.value(key).trimmed();
        for (const auto& value : values) {
            if (cell.compare(value, Qt::CaseInsensitive) == 0) {
                ++total;
                break;
            }
        }
    }
    return total;
}

QLabel* makeSectionText(const QString& text) {
    auto* label = new QLabel(text);
    label->setWordWrap(true);
    label->setStyleSheet("font-size:13px; color:#486581; line-height:1.4;");
    return label;
}
}

DepartmentDashboardsPage::DepartmentDashboardsPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(14);

    auto* title = new QLabel("Department Dashboards", this);
    title->setStyleSheet("font-size:24px; font-weight:800; color:#102a43;");
    auto* subtitle = new QLabel(
        "A role-friendly operating view that groups the current workload into Nursing, Admissions & Census, Reimbursement, and Facility Support so department leads can scan what matters fastest.",
        this);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("font-size:13px; color:#486581;");

    auto* refreshButton = new QPushButton("Refresh Department View", this);

    auto* headerRow = new QHBoxLayout();
    auto* textWrap = new QVBoxLayout();
    textWrap->addWidget(title);
    textWrap->addWidget(subtitle);
    headerRow->addLayout(textWrap, 1);
    headerRow->addWidget(refreshButton, 0, Qt::AlignTop);
    root->addLayout(headerRow);

    auto* cardsGrid = new QGridLayout();
    cardsGrid->setHorizontalSpacing(12);
    cardsGrid->setVerticalSpacing(12);
    cardsGrid->addWidget(new KpiCard("Nursing Department", QString::number(m_db->countWhere("staffing_assignments", "status='Open'")) + " open staffing"), 0, 0);
    cardsGrid->addWidget(new KpiCard("Admissions & Census", QString::number(m_db->countWhere("admissions", "status IN ('Pending','Planned','Ready','Needs Docs')")) + " active referrals"), 0, 1);
    cardsGrid->addWidget(new KpiCard("Revenue & MDS", QString::number(m_db->countWhere("mds_items", "status IN ('Open','Pending','In Progress','Watch')")) + " MDS items"), 0, 2);
    cardsGrid->addWidget(new KpiCard("Facility Support", QString::number(m_db->countWhere("environmental_rounds", "status IN ('Open','Watch','Escalated','In Progress')")) + " plant rounds open"), 0, 3);
    root->addLayout(cardsGrid);

    auto* detailsGrid = new QGridLayout();
    detailsGrid->setHorizontalSpacing(12);
    detailsGrid->setVerticalSpacing(12);

    auto* nursingBox = new QGroupBox("Nursing Operations", this);
    auto* nursingLayout = new QVBoxLayout(nursingBox);
    nursingSummaryLabel = makeSectionText("--");
    nursingLayout->addWidget(nursingSummaryLabel);

    auto* admissionsBox = new QGroupBox("Admissions & Census", this);
    auto* admissionsLayout = new QVBoxLayout(admissionsBox);
    admissionsSummaryLabel = makeSectionText("--");
    admissionsLayout->addWidget(admissionsSummaryLabel);
    admissionsTable = new QTableWidget(admissionsBox);
    admissionsTable->setColumnCount(4);
    admissionsTable->setHorizontalHeaderLabels({"Referral", "Planned", "Payer", "Status"});
    admissionsTable->horizontalHeader()->setStretchLastSection(true);
    admissionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    admissionsTable->verticalHeader()->setVisible(false);
    admissionsTable->setAlternatingRowColors(true);
    admissionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    admissionsTable->setSelectionMode(QAbstractItemView::NoSelection);
    admissionsLayout->addWidget(admissionsTable);

    auto* reimbursementBox = new QGroupBox("Reimbursement, MDS & Triple Check", this);
    auto* reimbursementLayout = new QVBoxLayout(reimbursementBox);
    reimbursementSummaryLabel = makeSectionText("--");
    reimbursementLayout->addWidget(reimbursementSummaryLabel);

    auto* supportBox = new QGroupBox("Facility Support Departments", this);
    auto* supportLayout = new QVBoxLayout(supportBox);
    supportSummaryLabel = makeSectionText("--");
    supportLayout->addWidget(supportSummaryLabel);
    supportTable = new QTableWidget(supportBox);
    supportTable->setColumnCount(4);
    supportTable->setHorizontalHeaderLabels({"Department", "Item", "Owner", "Status"});
    supportTable->horizontalHeader()->setStretchLastSection(true);
    supportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    supportTable->verticalHeader()->setVisible(false);
    supportTable->setAlternatingRowColors(true);
    supportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    supportTable->setSelectionMode(QAbstractItemView::NoSelection);
    supportLayout->addWidget(supportTable);

    detailsGrid->addWidget(nursingBox, 0, 0);
    detailsGrid->addWidget(admissionsBox, 0, 1);
    detailsGrid->addWidget(reimbursementBox, 1, 0);
    detailsGrid->addWidget(supportBox, 1, 1);
    root->addLayout(detailsGrid);

    setStyleSheet(styleSheet() + R"(
        QGroupBox {
            border: 1px solid #e4e7eb;
            border-radius: 16px;
            margin-top: 10px;
            font-weight: 700;
            color: #102a43;
            background: #ffffff;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
        }
    )");

    connect(refreshButton, &QPushButton::clicked, this, &DepartmentDashboardsPage::refreshData);
    refreshData();
}

void DepartmentDashboardsPage::refreshData() {
    const auto staffingAssignments = m_db->fetchTable("staffing_assignments", {"department", "shift_name", "role_name", "employee_name", "status"});
    const auto incidents = m_db->fetchTable("incidents", {"status"});
    const auto admissions = m_db->fetchTable("admissions", {"resident_name", "planned_date", "payer", "status"});
    const auto residents = m_db->fetchTable("residents", {"status"});
    const auto bedBoard = m_db->fetchTable("bed_board", {"status"});
    const auto managedCare = m_db->fetchTable("managed_care_items", {"status"});
    const auto mds = m_db->fetchTable("mds_items", {"status"});
    const auto docs = m_db->fetchTable("document_items", {"status", "module_name"});
    const auto dietary = m_db->fetchTable("dietary_items", {"item_name", "owner", "status"});
    const auto pharmacy = m_db->fetchTable("pharmacy_items", {"item_name", "owner", "status"});
    const auto environmental = m_db->fetchTable("environmental_rounds", {"round_item", "owner", "status"});
    const auto transport = m_db->fetchTable("transport_items", {"item_name", "owner", "status"});

    const int openNursing = countByValue(staffingAssignments, "status", "Open");
    const int incidentsOpen = countByValues(incidents, "status", {"Open", "In Progress", "Escalated"});
    const int pendingAdmissions = countByValues(admissions, "status", {"Pending", "Planned", "Ready", "Needs Docs"});
    const int currentResidents = countByValue(residents, "status", "Current");
    const int bedTurns = countByValues(bedBoard, "status", {"Open", "Held", "Turnover"});
    const int openManagedCare = countByValues(managedCare, "status", {"Open", "In Progress", "Watch"});
    const int openMds = countByValues(mds, "status", {"Open", "Pending", "In Progress", "Watch"});
    const int docsNeedingContext = countByValues(docs, "status", {"Needs Review", "Needs Context", "Open"});
    const int supportOpen = countByValues(dietary, "status", {"Open", "In Progress", "Watch"})
        + countByValues(pharmacy, "status", {"Open", "In Progress", "Watch"})
        + countByValues(environmental, "status", {"Open", "In Progress", "Watch", "Escalated"})
        + countByValues(transport, "status", {"Open", "In Progress", "Watch"});

    nursingSummaryLabel->setText(
        QString("Nursing can quickly see %1 open staffing assignments and %2 incident follow-ups still in motion. Use this view during stand-up to pair staffing gaps with resident-safety workload.")
            .arg(openNursing)
            .arg(incidentsOpen));

    admissionsSummaryLabel->setText(
        QString("Admissions and census teams currently have %1 active referrals, %2 current residents, and %3 bed-board items that still affect room readiness or census movement.")
            .arg(pendingAdmissions)
            .arg(currentResidents)
            .arg(bedTurns));

    reimbursementSummaryLabel->setText(
        QString("Reimbursement leaders are tracking %1 managed-care items, %2 MDS / triple-check records, and %3 document records that still need review or stronger context.")
            .arg(openManagedCare)
            .arg(openMds)
            .arg(docsNeedingContext));

    supportSummaryLabel->setText(
        QString("Support departments currently carry %1 open items across dietary, pharmacy, environmental rounds, and transportation — useful for morning huddle escalation and end-of-day cleanup.")
            .arg(supportOpen));

    populateAdmissionsTable();
    populateSupportTable();
}

void DepartmentDashboardsPage::populateAdmissionsTable() {
    const auto admissions = m_db->fetchTable("admissions", {"resident_name", "planned_date", "payer", "status"});
    admissionsTable->setRowCount(admissions.size());
    for (int r = 0; r < admissions.size(); ++r) {
        const auto& row = admissions[r];
        admissionsTable->setItem(r, 0, new QTableWidgetItem(row.value("resident_name")));
        admissionsTable->setItem(r, 1, new QTableWidgetItem(row.value("planned_date")));
        admissionsTable->setItem(r, 2, new QTableWidgetItem(row.value("payer")));
        admissionsTable->setItem(r, 3, new QTableWidgetItem(row.value("status")));
    }
}

void DepartmentDashboardsPage::populateSupportTable() {
    struct SupportRow { QString department; QString item; QString owner; QString status; };
    QList<SupportRow> rows;

    const auto dietary = m_db->fetchTable("dietary_items", {"item_name", "owner", "status"});
    for (const auto& row : dietary) rows.push_back({"Dietary", row.value("item_name"), row.value("owner"), row.value("status")});

    const auto pharmacy = m_db->fetchTable("pharmacy_items", {"item_name", "owner", "status"});
    for (const auto& row : pharmacy) rows.push_back({"Pharmacy", row.value("item_name"), row.value("owner"), row.value("status")});

    const auto environmental = m_db->fetchTable("environmental_rounds", {"issue_name", "owner", "status"});
    for (const auto& row : environmental) rows.push_back({"Environmental", row.value("issue_name"), row.value("owner"), row.value("status")});

    const auto transport = m_db->fetchTable("transport_items", {"appointment_type", "owner", "status"});
    for (const auto& row : transport) rows.push_back({"Transportation", row.value("appointment_type"), row.value("owner"), row.value("status")});

    supportTable->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        supportTable->setItem(r, 0, new QTableWidgetItem(rows[r].department));
        supportTable->setItem(r, 1, new QTableWidgetItem(rows[r].item));
        supportTable->setItem(r, 2, new QTableWidgetItem(rows[r].owner));
        supportTable->setItem(r, 3, new QTableWidgetItem(rows[r].status));
    }
}
