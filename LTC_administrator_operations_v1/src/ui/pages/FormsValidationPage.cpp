#include "FormsValidationPage.h"
#include "../../data/DatabaseManager.h"

#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
QFrame* makeCard(QWidget* parent) {
    auto* card = new QFrame(parent);
    card->setStyleSheet("QFrame { background:#ffffff; border:1px solid #d9e2ec; border-radius:18px; }");
    return card;
}

QLabel* makeStatCard(const QString& title, const QString& value, const QString& note, QWidget* parent) {
    auto* card = new QLabel(QString("<div style='font-size:12px; color:#486581; text-transform:uppercase; letter-spacing:0.8px;'>%1</div>"
                                     "<div style='font-size:26px; font-weight:700; color:#102a43; margin-top:4px;'>%2</div>"
                                     "<div style='font-size:12px; color:#627d98; margin-top:6px;'>%3</div>")
                            .arg(title, value, note), parent);
    card->setTextFormat(Qt::RichText);
    card->setWordWrap(true);
    card->setMinimumHeight(110);
    card->setStyleSheet("background:#f8fbff; border:1px solid #d9e2ec; border-radius:16px; padding:14px;");
    return card;
}
}

FormsValidationPage::FormsValidationPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(14);

    auto* heading = new QLabel("Advanced Forms & Validation", this);
    heading->setStyleSheet("font-size:24px; font-weight:700; color:#102a43;");
    auto* subheading = new QLabel(
        "v44 introduces a richer forms-and-validation workspace so admit, staffing, document, and MDS workflows can be completed with clearer required fields and fewer missing details.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color:#486581; font-size:13px;");
    root->addWidget(heading);
    root->addWidget(subheading);

    const auto alerts = m_db ? m_db->validationAlerts() : QList<QMap<QString, QString>>{};
    const int admissionsCount = m_db ? m_db->countWhere("admissions", "trim(ifnull(payer,'')) = '' OR trim(ifnull(diagnosis_summary,'')) = '' OR trim(ifnull(assessment_type,'')) = '' OR trim(ifnull(planned_date,'')) = ''") : 0;
    const int staffingOpen = m_db ? m_db->countWhere("staffing_assignments", "status = 'Open'") : 0;
    const int docsMissing = m_db ? m_db->countWhere("document_items", "trim(ifnull(owner,'')) = '' OR trim(ifnull(linked_item,'')) = '' OR trim(ifnull(document_type,'')) = ''") : 0;
    const int mdsMissing = m_db ? m_db->countWhere("mds_items", "trim(ifnull(ard_date,'')) = '' OR trim(ifnull(owner,'')) = ''") : 0;

    auto* statRow = new QHBoxLayout();
    statRow->setSpacing(12);
    statRow->addWidget(makeStatCard("Validation alerts", QString::number(alerts.size()), "Cross-module items missing required context or still open.", this));
    statRow->addWidget(makeStatCard("Admissions review", QString::number(admissionsCount), "Referral waitlist items missing intake details.", this));
    statRow->addWidget(makeStatCard("Docs needing context", QString::number(docsMissing), "Imported files missing owner, type, or linked workflow.", this));
    statRow->addWidget(makeStatCard("MDS gaps", QString::number(mdsMissing), "Clinical reimbursement items missing ARD or ownership.", this));
    root->addLayout(statRow);

    auto* checklistCard = makeCard(this);
    auto* checklistLayout = new QVBoxLayout(checklistCard);
    checklistLayout->setContentsMargins(18, 16, 18, 16);
    checklistLayout->setSpacing(10);
    auto* checklistTitle = new QLabel("Structured intake standards", checklistCard);
    checklistTitle->setStyleSheet("font-size:18px; font-weight:700; color:#102a43;");
    auto* checklistBody = new QLabel(
        "• Admissions: resident, referral source, payer, diagnosis summary, assessment type, planned date, room target.\n"
        "• Staffing: work date, department, shift, role, coverage status, minimum staffing review.\n"
        "• Documents: document name, type, linked item, owner, import path, due/next action context.\n"
        "• MDS: resident, payer, assessment type, ARD, owner, triple-check follow-up status.",
        checklistCard);
    checklistBody->setWordWrap(true);
    checklistBody->setStyleSheet("color:#486581; line-height:1.5;");
    checklistLayout->addWidget(checklistTitle);
    checklistLayout->addWidget(checklistBody);
    root->addWidget(checklistCard);

    auto* tableRow = new QHBoxLayout();
    tableRow->setSpacing(14);

    auto* profilesCard = makeCard(this);
    auto* profilesLayout = new QVBoxLayout(profilesCard);
    profilesLayout->setContentsMargins(18, 16, 18, 16);
    profilesLayout->setSpacing(10);
    auto* profilesTitle = new QLabel("Form profiles", profilesCard);
    profilesTitle->setStyleSheet("font-size:18px; font-weight:700; color:#102a43;");
    auto* profilesHint = new QLabel("Use these required-field profiles to standardize data entry across the app.", profilesCard);
    profilesHint->setWordWrap(true);
    profilesHint->setStyleSheet("color:#486581;");
    m_profilesTable = new QTableWidget(profilesCard);
    m_profilesTable->setColumnCount(4);
    m_profilesTable->setHorizontalHeaderLabels({"Module", "Required fields", "Form style", "Notes"});
    m_profilesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_profilesTable->verticalHeader()->setVisible(false);
    m_profilesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_profilesTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_profilesTable->setAlternatingRowColors(true);
    profilesLayout->addWidget(profilesTitle);
    profilesLayout->addWidget(profilesHint);
    profilesLayout->addWidget(m_profilesTable, 1);
    tableRow->addWidget(profilesCard, 1);

    auto* alertsCard = makeCard(this);
    auto* alertsLayout = new QVBoxLayout(alertsCard);
    alertsLayout->setContentsMargins(18, 16, 18, 16);
    alertsLayout->setSpacing(10);
    auto* alertsTitle = new QLabel("Validation queue", alertsCard);
    alertsTitle->setStyleSheet("font-size:18px; font-weight:700; color:#102a43;");
    auto* alertsHint = new QLabel("Operational items that still need required context before they are truly complete.", alertsCard);
    alertsHint->setWordWrap(true);
    alertsHint->setStyleSheet("color:#486581;");
    m_alertsTable = new QTableWidget(alertsCard);
    m_alertsTable->setColumnCount(4);
    m_alertsTable->setHorizontalHeaderLabels({"Module", "Item", "Issue", "Recommended action"});
    m_alertsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_alertsTable->verticalHeader()->setVisible(false);
    m_alertsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_alertsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_alertsTable->setAlternatingRowColors(true);
    alertsLayout->addWidget(alertsTitle);
    alertsLayout->addWidget(alertsHint);
    alertsLayout->addWidget(m_alertsTable, 1);
    tableRow->addWidget(alertsCard, 1);

    root->addLayout(tableRow, 1);

    loadProfiles();
    loadAlerts();
}

void FormsValidationPage::loadProfiles() {
    if (!m_db || !m_profilesTable) return;
    const auto rows = m_db->fetchTable("validation_profiles", {"module_name", "required_fields", "form_style", "notes"});
    m_profilesTable->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows.at(r);
        m_profilesTable->setItem(r, 0, new QTableWidgetItem(row.value("module_name")));
        m_profilesTable->setItem(r, 1, new QTableWidgetItem(row.value("required_fields")));
        m_profilesTable->setItem(r, 2, new QTableWidgetItem(row.value("form_style")));
        m_profilesTable->setItem(r, 3, new QTableWidgetItem(row.value("notes")));
    }
}

void FormsValidationPage::loadAlerts() {
    if (!m_db || !m_alertsTable) return;
    const auto rows = m_db->validationAlerts();
    m_alertsTable->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows.at(r);
        m_alertsTable->setItem(r, 0, new QTableWidgetItem(row.value("module_name")));
        m_alertsTable->setItem(r, 1, new QTableWidgetItem(row.value("item_name")));
        m_alertsTable->setItem(r, 2, new QTableWidgetItem(row.value("issue_name")));
        m_alertsTable->setItem(r, 3, new QTableWidgetItem(row.value("recommended_action")));
    }
}
