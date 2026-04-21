#include "SopCenterPage.h"
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

SopCenterPage::SopCenterPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(12);

    auto* title = new QLabel("SOP / Quick Start Center", this);
    title->setStyleSheet("font-size:20px; font-weight:700;");
    root->addWidget(title);

    m_summary = new QLabel(this);
    m_summary->setWordWrap(true);
    root->addWidget(m_summary);

    auto* formBox = new QGroupBox("Add SOP or quick-start item", this);
    auto* form = new QFormLayout(formBox);
    m_areaEdit = new QLineEdit(this);
    m_titleEdit = new QLineEdit(this);
    m_ownerEdit = new QLineEdit(this);
    m_statusEdit = new QLineEdit(this);
    m_lastReviewedEdit = new QLineEdit(this);
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setMinimumHeight(70);

    m_areaEdit->setPlaceholderText("Admissions / Staffing / Reports / Backup / Survey");
    m_titleEdit->setPlaceholderText("Referral waitlist to admit quick start");
    m_ownerEdit->setPlaceholderText("Administrator / DON / Admissions / IT");
    m_statusEdit->setPlaceholderText("Active / Watch / Needs Update");
    m_lastReviewedEdit->setPlaceholderText("YYYY-MM-DD");

    form->addRow("Area", m_areaEdit);
    form->addRow("Title", m_titleEdit);
    form->addRow("Owner", m_ownerEdit);
    form->addRow("Status", m_statusEdit);
    form->addRow("Last reviewed", m_lastReviewedEdit);
    form->addRow("Notes", m_notesEdit);

    auto* buttons = new QHBoxLayout();
    auto* addButton = new QPushButton("Add SOP Item", this);
    m_seedButton = new QPushButton("Seed Default SOPs", this);
    m_refreshButton = new QPushButton("Refresh", this);
    buttons->addWidget(addButton);
    buttons->addWidget(m_seedButton);
    buttons->addWidget(m_refreshButton);
    buttons->addStretch();
    form->addRow(buttons);
    root->addWidget(formBox);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"Area", "Title", "Owner", "Status", "Last reviewed"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    root->addWidget(m_table, 1);

    auto* notesBox = new QGroupBox("Why this matters", this);
    auto* notesLayout = new QVBoxLayout(notesBox);
    auto* notes = new QTextEdit(this);
    notes->setReadOnly(true);
    notes->setPlainText(
        "v51 adds a dedicated SOP / Quick Start Center so rollout knowledge stops living only in memory or scattered notes.\n\n"        "Use this page for the small operating guides people actually need every day: admissions waitlist to admit, staffing minimum review, backup and restore steps, report exports, document intake, and survey command prep.\n\n"        "This makes the app easier to hand off, easier to train on, and easier to keep consistent after deployment.");
    notesLayout->addWidget(notes);
    root->addWidget(notesBox);

    connect(addButton, &QPushButton::clicked, this, &SopCenterPage::addSopItem);
    connect(m_seedButton, &QPushButton::clicked, this, &SopCenterPage::seedDefaultRows);
    connect(m_refreshButton, &QPushButton::clicked, this, &SopCenterPage::refreshData);

    refreshData();
}

void SopCenterPage::addSopItem() {
    if (!m_db) return;
    m_db->addRecord("sop_items", {
        {"area_name", m_areaEdit->text().trimmed()},
        {"title_name", m_titleEdit->text().trimmed()},
        {"owner_name", m_ownerEdit->text().trimmed()},
        {"status", m_statusEdit->text().trimmed()},
        {"last_reviewed", m_lastReviewedEdit->text().trimmed()},
        {"notes", m_notesEdit->toPlainText().trimmed()}
    });
    m_areaEdit->clear();
    m_titleEdit->clear();
    m_ownerEdit->clear();
    m_statusEdit->clear();
    m_lastReviewedEdit->clear();
    m_notesEdit->clear();
    refreshData();
}

void SopCenterPage::seedDefaultRows() {
    if (!m_db) return;
    m_db->addRecord("sop_items", {{"area_name","Admissions"}, {"title_name","Referral waitlist to admit quick start"}, {"owner_name","Admissions Director"}, {"status","Active"}, {"last_reviewed","2026-04-20"}, {"notes","Review referral, import documents, push MDS/diagnosis, mark ready, then admit from waitlist."}});
    m_db->addRecord("sop_items", {{"area_name","Staffing"}, {"title_name","Minimum staffing review before shift change"}, {"owner_name","Staffing Coordinator"}, {"status","Active"}, {"last_reviewed","2026-04-20"}, {"notes","Check open assignments, minimum gaps, uncovered hours, and HPRD snapshot before finalizing coverage."}});
    m_db->addRecord("sop_items", {{"area_name","Reports"}, {"title_name","Daily executive summary and CSV export"}, {"owner_name","Administrator"}, {"status","Watch"}, {"last_reviewed","2026-04-19"}, {"notes","Run summary preview, export census/staffing CSV, and print if leadership rounding packet is needed."}});
    m_db->addRecord("sop_items", {{"area_name","Backup"}, {"title_name","Create backup before restore or major cleanup"}, {"owner_name","IT"}, {"status","Active"}, {"last_reviewed","2026-04-18"}, {"notes","Create timestamped backup, confirm backup folder, then restore only when users are out of the app."}});
    refreshData();
}

void SopCenterPage::refreshData() {
    if (!m_db) return;
    const QStringList cols = {"area_name","title_name","owner_name","status","last_reviewed"};
    const auto rows = m_db->fetchTable("sop_items", cols + QStringList{"notes"});
    m_table->setRowCount(rows.size());
    int active = 0;
    int watch = 0;
    int needsUpdate = 0;
    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];
        for (int c = 0; c < cols.size(); ++c) {
            m_table->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
        }
        const auto status = row.value("status").trimmed().toLower();
        if (status == "active") ++active;
        else if (status == "watch") ++watch;
        else ++needsUpdate;
    }
    m_summary->setText(QString("SOP items: %1 · Active: %2 · Watch: %3 · Needs update: %4. Use this to keep rollout knowledge and day-to-day operating steps in one place.")
        .arg(rows.size()).arg(active).arg(watch).arg(needsUpdate));
}
