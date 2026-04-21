#include "ExternalSyncReadinessPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

ExternalSyncReadinessPage::ExternalSyncReadinessPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(12);

    auto* title = new QLabel("External Sync / EMR Readiness", this);
    title->setStyleSheet("font-size:20px; font-weight:700;");
    root->addWidget(title);

    m_summary = new QLabel(this);
    m_summary->setWordWrap(true);
    root->addWidget(m_summary);

    auto* formBox = new QGroupBox("Add sync profile", this);
    auto* form = new QFormLayout(formBox);
    m_systemNameEdit = new QLineEdit(this);
    m_entityTypeEdit = new QLineEdit(this);
    m_directionEdit = new QLineEdit(this);
    m_statusEdit = new QLineEdit(this);
    m_ownerEdit = new QLineEdit(this);
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setMinimumHeight(70);
    m_systemNameEdit->setPlaceholderText("PointClickCare / EMR / Billing Export");
    m_entityTypeEdit->setPlaceholderText("Resident census / Referral intake / MDS / Documents");
    m_directionEdit->setPlaceholderText("Import / Export / Bidirectional");
    m_statusEdit->setPlaceholderText("Planned / Watch / Ready");
    m_ownerEdit->setPlaceholderText("Administrator / Admissions / MDS Coordinator");
    form->addRow("System", m_systemNameEdit);
    form->addRow("Entity type", m_entityTypeEdit);
    form->addRow("Direction", m_directionEdit);
    form->addRow("Status", m_statusEdit);
    form->addRow("Owner", m_ownerEdit);
    form->addRow("Notes", m_notesEdit);

    auto* buttons = new QHBoxLayout();
    auto* addButton = new QPushButton("Add Sync Profile", this);
    m_seedButton = new QPushButton("Seed Default Syncs", this);
    m_refreshButton = new QPushButton("Refresh", this);
    buttons->addWidget(addButton);
    buttons->addWidget(m_seedButton);
    buttons->addWidget(m_refreshButton);
    buttons->addStretch();
    form->addRow(buttons);
    root->addWidget(formBox);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"System", "Entity", "Direction", "Status", "Owner", "Notes"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    root->addWidget(m_table, 1);

    auto* notesBox = new QGroupBox("Why this matters", this);
    auto* notesLayout = new QVBoxLayout(notesBox);
    auto* notes = new QTextEdit(this);
    notes->setReadOnly(true);
    notes->setPlainText(
        "v49 keeps the single-facility focus and adds a cleaner external-sync foundation.\n\n"
        "Use this workspace to document which operational data streams are ready for future import/export work, including referral intake, resident census, MDS timing, diagnosis carry-forward, and document handoff.\n\n"
        "This keeps future PCC/EMR work visible without forcing a live integration before you want it.");
    notesLayout->addWidget(notes);
    root->addWidget(notesBox);

    connect(addButton, &QPushButton::clicked, this, &ExternalSyncReadinessPage::addProfile);
    connect(m_seedButton, &QPushButton::clicked, this, &ExternalSyncReadinessPage::seedDefaultRows);
    connect(m_refreshButton, &QPushButton::clicked, this, &ExternalSyncReadinessPage::refreshData);

    refreshData();
}

void ExternalSyncReadinessPage::addProfile() {
    if (!m_db) return;
    m_db->addRecord("external_sync_profiles", {
        {"system_name", m_systemNameEdit->text().trimmed()},
        {"entity_type", m_entityTypeEdit->text().trimmed()},
        {"sync_direction", m_directionEdit->text().trimmed()},
        {"status", m_statusEdit->text().trimmed()},
        {"owner", m_ownerEdit->text().trimmed()},
        {"notes", m_notesEdit->toPlainText().trimmed()}
    });
    m_systemNameEdit->clear();
    m_entityTypeEdit->clear();
    m_directionEdit->clear();
    m_statusEdit->clear();
    m_ownerEdit->clear();
    m_notesEdit->clear();
    refreshData();
}

void ExternalSyncReadinessPage::seedDefaultRows() {
    if (!m_db) return;
    m_db->addRecord("external_sync_profiles", {{"system_name","PointClickCare Intake"}, {"entity_type","Referral intake + admit-ready census"}, {"sync_direction","Import"}, {"status","Planned"}, {"owner","Admissions"}, {"notes","Target future import of referral demographics, payer source, and admit-readiness status."}});
    m_db->addRecord("external_sync_profiles", {{"system_name","Clinical Reimbursement Feed"}, {"entity_type","MDS / ARD / Triple Check"}, {"sync_direction","Bidirectional"}, {"status","Watch"}, {"owner","MDS Coordinator"}, {"notes","Future handoff path for MDS timing, payer review, and reimbursement checkpoints."}});
    m_db->addRecord("external_sync_profiles", {{"system_name","Document Exchange"}, {"entity_type","Referral packet / document intake"}, {"sync_direction","Import"}, {"status","Ready"}, {"owner","Operations"}, {"notes","Local document intake is already in place; next step is mapping external file metadata."}});
    m_db->addRecord("external_sync_profiles", {{"system_name","Executive Reporting Export"}, {"entity_type","Daily summary / census / staffing"}, {"sync_direction","Export"}, {"status","Ready"}, {"owner","Administrator"}, {"notes","Current local export path can be promoted later to scheduled outbound reporting."}});
    refreshData();
}

void ExternalSyncReadinessPage::refreshData() {
    if (!m_db) return;
    const QStringList cols = {"system_name","entity_type","sync_direction","status","owner","notes"};
    const auto rows = m_db->fetchTable("external_sync_profiles", cols);
    m_table->setRowCount(rows.size());
    int ready = 0;
    int planned = 0;
    int watch = 0;
    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];
        for (int c = 0; c < cols.size(); ++c) {
            m_table->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
        }
        const auto status = row.value("status").trimmed().toLower();
        if (status == "ready") ++ready;
        else if (status == "planned") ++planned;
        else if (status == "watch") ++watch;
    }
    m_summary->setText(QString("Tracked sync profiles: %1 · Ready: %2 · Planned: %3 · Watch: %4. This keeps future EMR/PCC work visible while preserving the current local workflow.")
        .arg(rows.size()).arg(ready).arg(planned).arg(watch));
}
