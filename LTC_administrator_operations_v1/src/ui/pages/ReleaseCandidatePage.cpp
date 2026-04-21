#include "ReleaseCandidatePage.h"
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

ReleaseCandidatePage::ReleaseCandidatePage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(12);

    auto* title = new QLabel("Release Candidate / Deployment Readiness", this);
    title->setStyleSheet("font-size:20px; font-weight:700;");
    root->addWidget(title);

    m_summary = new QLabel(this);
    m_summary->setWordWrap(true);
    root->addWidget(m_summary);

    auto* formBox = new QGroupBox("Add deployment readiness item", this);
    auto* form = new QFormLayout(formBox);
    m_areaEdit = new QLineEdit(this);
    m_itemEdit = new QLineEdit(this);
    m_ownerEdit = new QLineEdit(this);
    m_statusEdit = new QLineEdit(this);
    m_dueDateEdit = new QLineEdit(this);
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setMinimumHeight(70);
    m_areaEdit->setPlaceholderText("Build / Packaging / Validation / Training / Release Notes");
    m_itemEdit->setPlaceholderText("Create release zip and smoke test on clean machine");
    m_ownerEdit->setPlaceholderText("Administrator / IT / DON / Admissions");
    m_statusEdit->setPlaceholderText("Open / Watch / Ready / Complete");
    m_dueDateEdit->setPlaceholderText("YYYY-MM-DD");
    form->addRow("Area", m_areaEdit);
    form->addRow("Item", m_itemEdit);
    form->addRow("Owner", m_ownerEdit);
    form->addRow("Status", m_statusEdit);
    form->addRow("Due date", m_dueDateEdit);
    form->addRow("Notes", m_notesEdit);

    auto* buttons = new QHBoxLayout();
    auto* addButton = new QPushButton("Add Checklist Item", this);
    m_seedButton = new QPushButton("Seed Default Checklist", this);
    m_refreshButton = new QPushButton("Refresh", this);
    buttons->addWidget(addButton);
    buttons->addWidget(m_seedButton);
    buttons->addWidget(m_refreshButton);
    buttons->addStretch();
    form->addRow(buttons);
    root->addWidget(formBox);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"Area", "Item", "Owner", "Status", "Due", "Notes"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    root->addWidget(m_table, 1);

    auto* notesBox = new QGroupBox("Release focus", this);
    auto* notesLayout = new QVBoxLayout(notesBox);
    auto* notes = new QTextEdit(this);
    notes->setReadOnly(true);
    notes->setPlainText(
        "v51 keeps the polished single-facility release-candidate snapshot and adds an operations playbook layer for training and repeatable workflows.\n\n"
        "Use this page to track whether the app is actually ready to hand to another machine or another leader: build validation, backup verification, login test, export test, document intake test, referral-to-admit workflow test, and end-user training notes.\n\n"
        "This keeps the rollout work visible in one place instead of hiding it in ad-hoc notes.");
    notesLayout->addWidget(notes);
    root->addWidget(notesBox);

    connect(addButton, &QPushButton::clicked, this, &ReleaseCandidatePage::addChecklistItem);
    connect(m_seedButton, &QPushButton::clicked, this, &ReleaseCandidatePage::seedDefaultRows);
    connect(m_refreshButton, &QPushButton::clicked, this, &ReleaseCandidatePage::refreshData);

    refreshData();
}

void ReleaseCandidatePage::addChecklistItem() {
    if (!m_db) return;
    m_db->addRecord("release_candidate_items", {
        {"area_name", m_areaEdit->text().trimmed()},
        {"item_name", m_itemEdit->text().trimmed()},
        {"owner_name", m_ownerEdit->text().trimmed()},
        {"status", m_statusEdit->text().trimmed()},
        {"due_date", m_dueDateEdit->text().trimmed()},
        {"notes", m_notesEdit->toPlainText().trimmed()}
    });
    m_areaEdit->clear();
    m_itemEdit->clear();
    m_ownerEdit->clear();
    m_statusEdit->clear();
    m_dueDateEdit->clear();
    m_notesEdit->clear();
    refreshData();
}

void ReleaseCandidatePage::seedDefaultRows() {
    if (!m_db) return;
    m_db->addRecord("release_candidate_items", {{"area_name","Build"}, {"item_name","Validate Release build on Qt 6.11 MinGW"}, {"owner_name","IT"}, {"status","Ready"}, {"due_date","2026-04-22"}, {"notes","Clean build script should complete without manual edits."}});
    m_db->addRecord("release_candidate_items", {{"area_name","Packaging"}, {"item_name","Verify runtime deployment and launch from fresh folder"}, {"owner_name","IT"}, {"status","Watch"}, {"due_date","2026-04-22"}, {"notes","Confirm Qt runtime deployment and local SQLite creation."}});
    m_db->addRecord("release_candidate_items", {{"area_name","Workflow Validation"}, {"item_name","Test referral waitlist to MDS/diagnosis and admit flow"}, {"owner_name","Admissions"}, {"status","Open"}, {"due_date","2026-04-23"}, {"notes","Use one referral with imported document and admit from waitlist end to end."}});
    m_db->addRecord("release_candidate_items", {{"area_name","Training"}, {"item_name","Prepare one-page end-user quick start"}, {"owner_name","Administrator"}, {"status","Open"}, {"due_date","2026-04-24"}, {"notes","Cover login, dashboard, referrals, staffing, reports, and backup/restore."}});
    refreshData();
}

void ReleaseCandidatePage::refreshData() {
    if (!m_db) return;
    const QStringList cols = {"area_name","item_name","owner_name","status","due_date","notes"};
    const auto rows = m_db->fetchTable("release_candidate_items", cols);
    m_table->setRowCount(rows.size());
    int ready = 0;
    int open = 0;
    int watch = 0;
    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];
        for (int c = 0; c < cols.size(); ++c) {
            m_table->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
        }
        const auto status = row.value("status").trimmed().toLower();
        if (status == "ready" || status == "complete") ++ready;
        else if (status == "watch") ++watch;
        else ++open;
    }
    m_summary->setText(QString("Release-readiness items: %1 · Ready/complete: %2 · Watch: %3 · Open: %4. Use this as the rollout checklist for the current single-facility build.")
        .arg(rows.size()).arg(ready).arg(watch).arg(open));
}
