
#include "ServiceLayerPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

ServiceLayerPage::ServiceLayerPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(12);

    auto* title = new QLabel("Service Layer / Integration Readiness", this);
    title->setStyleSheet("font-size:20px; font-weight:700;");
    root->addWidget(title);

    m_summary = new QLabel(this);
    m_summary->setWordWrap(true);
    root->addWidget(m_summary);

    auto* controls = new QHBoxLayout();
    m_seedButton = new QPushButton("Seed Default Services", this);
    m_refreshButton = new QPushButton("Refresh", this);
    controls->addWidget(m_seedButton);
    controls->addWidget(m_refreshButton);
    controls->addStretch();
    root->addLayout(controls);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"Service", "Purpose", "Status", "Owner", "Notes"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    root->addWidget(m_table, 1);

    auto* notesBox = new QGroupBox("Why this matters", this);
    auto* notesLayout = new QVBoxLayout(notesBox);
    m_notes = new QTextEdit(this);
    m_notes->setReadOnly(true);
    m_notes->setPlainText(
        "v48 focuses on service-layer cleanup instead of adding another major workflow.\n\n"
        "It gives you a central place to see which operational services already exist, what they support, and which ones are ready for future integrations like PCC/EMR import, document sync, or API-based reporting.\n\n"
        "This version is meant to make later builds easier to maintain without removing any current functionality.");
    notesLayout->addWidget(m_notes);
    root->addWidget(notesBox);

    connect(m_seedButton, &QPushButton::clicked, this, &ServiceLayerPage::seedDefaultRows);
    connect(m_refreshButton, &QPushButton::clicked, this, &ServiceLayerPage::refreshData);

    refreshData();
}

void ServiceLayerPage::seedDefaultRows() {
    if (!m_db) return;
    m_db->addRecord("service_registry", {{"service_name","AdmissionsPipelineService"}, {"purpose","Waitlist intake, admit-from-referral, MDS carry-forward"}, {"status","Ready"}, {"owner","Admissions"}, {"notes","Supports current referral-to-admit workflow."}});
    m_db->addRecord("service_registry", {{"service_name","StaffingCoverageService"}, {"purpose","Assignments, minimum staffing, HPRD summaries"}, {"status","Ready"}, {"owner","Staffing"}, {"notes","Backs current staffing coverage workflows."}});
    m_db->addRecord("service_registry", {{"service_name","DocumentIngestionService"}, {"purpose","Imports local files and links them to operational records"}, {"status","Watch"}, {"owner","Operations"}, {"notes","Local file workflow is ready; external sync can come later."}});
    m_db->addRecord("service_registry", {{"service_name","ReportingExportService"}, {"purpose","Daily summary, CSV export, print support"}, {"status","Ready"}, {"owner","Administrator"}, {"notes","Backs reports/export workflow."}});
    refreshData();
}

void ServiceLayerPage::refreshData() {
    if (!m_db) return;
    const auto rows = m_db->fetchTable("service_registry", {"service_name","purpose","status","owner","notes"});
    m_table->setRowCount(rows.size());
    int ready = 0;
    int watch = 0;
    const QStringList cols = {"service_name","purpose","status","owner","notes"};
    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];
        for (int c = 0; c < cols.size(); ++c) {
            m_table->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
        }
        const auto status = row.value("status").trimmed().toLower();
        if (status == "ready") ++ready;
        if (status == "watch") ++watch;
    }
    m_summary->setText(QString("Tracked services: %1 · Ready: %2 · Watch: %3. This workspace helps keep the app modular while preserving every current module and workflow.")
        .arg(rows.size()).arg(ready).arg(watch));
}
