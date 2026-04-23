#include "SharedNotesFollowUpPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDate>
#include <QDateTime>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {
QString normalizeContext(const QString& label) {
    const QString lowered = label.trimmed().toLower();
    if (lowered.startsWith("daily")) return "daily";
    if (lowered.startsWith("survey")) return "survey";
    if (lowered.startsWith("resident")) return "resident";
    if (lowered.startsWith("operations")) return "ops";
    if (lowered.startsWith("documents")) return "docs";
    return "global";
}

QTableWidgetItem* makeItem(const QString& text) {
    auto* item = new QTableWidgetItem(text);
    item->setToolTip(text);
    return item;
}
}

SharedNotesFollowUpPage::SharedNotesFollowUpPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Shared Notes & Follow-Up Threading", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v90 adds a shared follow-up workspace so connected records can carry one running note trail across daily operations, survey management, resident-care, and support workflows instead of scattering handoff details across separate tabs.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet(
        "background:#eef4f8; border:1px solid #d9e2ec; border-radius:10px;"
        "padding:8px 14px; color:#334e68; font-weight:600;");

    root->addWidget(heading);
    root->addWidget(subheading);
    root->addWidget(m_summaryLabel);

    auto* strip = new QHBoxLayout();
    auto makeStat = [&](const QString& title, QLabel*& valueLabel, const QString& hintText) {
        auto* card = new QGroupBox(title, this);
        auto* layout = new QVBoxLayout(card);
        valueLabel = new QLabel("0", card);
        valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #12344d;");
        auto* hint = new QLabel(hintText, card);
        hint->setWordWrap(true);
        hint->setStyleSheet("color:#5b6472;");
        layout->addWidget(valueLabel);
        layout->addWidget(hint);
        strip->addWidget(card);
    };
    makeStat("Open threads", m_openLabel, "Connected items that still need follow-up notes, handoff context, or active leadership review.");
    makeStat("Handoff status", m_handoffLabel, "Threads currently waiting on another owner, department, or next-step handoff.");
    makeStat("Touched today", m_recentLabel, "Threads updated today so leadership can see what recently changed across connected workspaces.");
    makeStat("Closed threads", m_closedLabel, "Threads already closed after the follow-up loop, note trail, or handoff was completed.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Add shared follow-up thread", this);
    auto* formLayout = new QFormLayout(formCard);
    m_contextCombo = new QComboBox(formCard);
    m_contextCombo->addItems({"Global", "Daily Operations", "Survey Management", "Resident Care", "Operations Support", "Documents / Reports"});
    m_sourceEdit = new QLineEdit(formCard);
    m_sourceEdit->setPlaceholderText("Issue or record label that should stay connected across tabs.");
    m_tabsEdit = new QLineEdit(formCard);
    m_tabsEdit->setPlaceholderText("Examples: Alerts, Morning Meeting, Unified Action Center");
    m_ownerEdit = new QLineEdit("Administrator", formCard);
    m_priorityCombo = new QComboBox(formCard);
    m_priorityCombo->addItems({"Critical", "High", "Medium", "Low"});
    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Needs Handoff", "Watching", "Closed"});
    m_noteEdit = new QTextEdit(formCard);
    m_noteEdit->setPlaceholderText("Running note trail, what changed, what is blocking progress, or what leadership should know.");
    m_noteEdit->setMinimumHeight(80);
    m_followupEdit = new QTextEdit(formCard);
    m_followupEdit->setPlaceholderText("Next step, owner handoff, requested follow-up, or check-back instruction.");
    m_followupEdit->setMinimumHeight(80);

    formLayout->addRow("Workspace context", m_contextCombo);
    formLayout->addRow("Connected item", m_sourceEdit);
    formLayout->addRow("Visible in tabs", m_tabsEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Priority", m_priorityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Shared notes", m_noteEdit);
    formLayout->addRow("Follow-up / handoff", m_followupEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Add shared thread", formCard);
    m_updateButton = new QPushButton("Mark selected updated", formCard);
    m_closeButton = new QPushButton("Mark selected closed", formCard);
    m_deleteButton = new QPushButton("Delete selected", formCard);
    m_refreshButton = new QPushButton("Refresh", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_updateButton);
    buttonRow->addWidget(m_closeButton);
    buttonRow->addWidget(m_deleteButton);
    buttonRow->addStretch(1);
    buttonRow->addWidget(m_refreshButton);
    formLayout->addRow(buttonRow);

    root->addWidget(formCard);

    auto* lower = new QHBoxLayout();

    auto* linksCard = new QGroupBox("Shared-record links already spanning tabs", this);
    auto* linksLayout = new QVBoxLayout(linksCard);
    m_sharedLinksTable = new QTableWidget(linksCard);
    m_sharedLinksTable->setColumnCount(5);
    m_sharedLinksTable->setHorizontalHeaderLabels({"Context", "Item", "Linked tabs", "Owner", "Status"});
    m_sharedLinksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_sharedLinksTable->verticalHeader()->setVisible(false);
    m_sharedLinksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_sharedLinksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_sharedLinksTable->setSelectionMode(QAbstractItemView::SingleSelection);
    linksLayout->addWidget(m_sharedLinksTable);
    lower->addWidget(linksCard, 1);

    auto* threadsCard = new QGroupBox("Shared thread history", this);
    auto* threadsLayout = new QVBoxLayout(threadsCard);
    m_threadsTable = new QTableWidget(threadsCard);
    m_threadsTable->setColumnCount(8);
    m_threadsTable->setHorizontalHeaderLabels({"ID", "Context", "Item", "Visible in tabs", "Owner", "Priority", "Status", "Last touched"});
    m_threadsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_threadsTable->verticalHeader()->setVisible(false);
    m_threadsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_threadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_threadsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    threadsLayout->addWidget(m_threadsTable);
    lower->addWidget(threadsCard, 2);

    root->addLayout(lower, 1);

    connect(m_addButton, &QPushButton::clicked, this, &SharedNotesFollowUpPage::addThread);
    connect(m_updateButton, &QPushButton::clicked, this, &SharedNotesFollowUpPage::markSelectedUpdated);
    connect(m_closeButton, &QPushButton::clicked, this, &SharedNotesFollowUpPage::markSelectedClosed);
    connect(m_deleteButton, &QPushButton::clicked, this, &SharedNotesFollowUpPage::deleteSelected);
    connect(m_refreshButton, &QPushButton::clicked, this, &SharedNotesFollowUpPage::refreshBoard);

    if (m_db) {
        connect(m_db, &DatabaseManager::dataChanged, this, &SharedNotesFollowUpPage::refreshBoard);
    }

    refreshBoard();
}

void SharedNotesFollowUpPage::populateSharedLinks() {
    QStringList contexts = {"daily", "survey", "resident", "ops", "docs"};
    QList<QMap<QString, QString>> rows;
    for (const QString& context : contexts) {
        const auto batch = m_db->sharedRecordLinksForContext(context);
        for (const auto& row : batch) rows.append(row);
    }

    m_sharedLinksTable->setRowCount(rows.size());
    int rowIndex = 0;
    for (const auto& row : rows) {
        QString context = row.value("context_key");
        if (context == "ops") context = "operations";
        m_sharedLinksTable->setItem(rowIndex, 0, makeItem(context));
        m_sharedLinksTable->setItem(rowIndex, 1, makeItem(row.value("source_label")));
        m_sharedLinksTable->setItem(rowIndex, 2, makeItem(row.value("linked_tabs")));
        m_sharedLinksTable->setItem(rowIndex, 3, makeItem(row.value("owner")));
        m_sharedLinksTable->setItem(rowIndex, 4, makeItem(row.value("status")));
        ++rowIndex;
    }
}

void SharedNotesFollowUpPage::populateThreads() {
    const auto rows = m_db->fetchTable(
        "shared_followup_threads",
        {"id", "context_key", "source_label", "linked_tabs", "owner_name", "priority", "status", "note_text", "followup_text", "last_touched"});

    m_threadsTable->setRowCount(rows.size());
    int rowIndex = 0;
    for (const auto& row : rows) {
        const QString tooltip = QString("Notes: %1\n\nFollow-up: %2").arg(row.value("note_text"), row.value("followup_text"));
        m_threadsTable->setItem(rowIndex, 0, makeItem(row.value("id")));
        auto* ctx = makeItem(row.value("context_key"));
        ctx->setToolTip(tooltip);
        m_threadsTable->setItem(rowIndex, 1, ctx);
        auto* item = makeItem(row.value("source_label"));
        item->setToolTip(tooltip);
        m_threadsTable->setItem(rowIndex, 2, item);
        m_threadsTable->setItem(rowIndex, 3, makeItem(row.value("linked_tabs")));
        m_threadsTable->setItem(rowIndex, 4, makeItem(row.value("owner_name")));
        m_threadsTable->setItem(rowIndex, 5, makeItem(row.value("priority")));
        m_threadsTable->setItem(rowIndex, 6, makeItem(row.value("status")));
        m_threadsTable->setItem(rowIndex, 7, makeItem(row.value("last_touched")));
        ++rowIndex;
    }
    m_threadsTable->setColumnHidden(0, true);
}

void SharedNotesFollowUpPage::refreshBoard() {
    populateSharedLinks();
    populateThreads();

    const int openCount = m_db->countWhere("shared_followup_threads", "status!='Closed'");
    const int handoffCount = m_db->countWhere("shared_followup_threads", "status='Needs Handoff'");
    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const int touchedToday = m_db->countWhere("shared_followup_threads", QString("substr(last_touched,1,10)='%1'").arg(today));
    const int closedCount = m_db->countWhere("shared_followup_threads", "status='Closed'");

    m_openLabel->setText(QString::number(openCount));
    m_handoffLabel->setText(QString::number(handoffCount));
    m_recentLabel->setText(QString::number(touchedToday));
    m_closedLabel->setText(QString::number(closedCount));
    m_summaryLabel->setText(
        QString("Shared follow-up threads keep one running note trail across connected workspaces. %1 thread(s) are still open, %2 need a handoff, and %3 were touched today.")
            .arg(openCount)
            .arg(handoffCount)
            .arg(touchedToday));
}

void SharedNotesFollowUpPage::addThread() {
    if (!m_db) return;
    if (m_sourceEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing item", "Enter the connected item or issue label before saving the thread.");
        return;
    }

    QMap<QString, QString> values;
    values["context_key"] = normalizeContext(m_contextCombo->currentText());
    values["source_label"] = m_sourceEdit->text().trimmed();
    values["linked_tabs"] = m_tabsEdit->text().trimmed();
    values["owner_name"] = m_ownerEdit->text().trimmed();
    values["priority"] = m_priorityCombo->currentText();
    values["status"] = m_statusCombo->currentText();
    values["note_text"] = m_noteEdit->toPlainText().trimmed();
    values["followup_text"] = m_followupEdit->toPlainText().trimmed();
    values["last_touched"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");

    if (!m_db->addRecord("shared_followup_threads", values)) {
        QMessageBox::warning(this, "Save failed", "The shared follow-up thread could not be saved.");
        return;
    }

    m_sourceEdit->clear();
    m_tabsEdit->clear();
    m_ownerEdit->setText("Administrator");
    m_priorityCombo->setCurrentText("High");
    m_statusCombo->setCurrentText("Open");
    m_noteEdit->clear();
    m_followupEdit->clear();
    refreshBoard();
}

void SharedNotesFollowUpPage::markSelectedUpdated() {
    if (!m_db) return;
    const int row = m_threadsTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Select a thread", "Select a shared follow-up thread first.");
        return;
    }
    const int id = m_threadsTable->item(row, 0)->text().toInt();
    QMap<QString, QString> values;
    values["status"] = "Watching";
    values["last_touched"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    if (!m_followupEdit->toPlainText().trimmed().isEmpty()) values["followup_text"] = m_followupEdit->toPlainText().trimmed();
    if (!m_noteEdit->toPlainText().trimmed().isEmpty()) values["note_text"] = m_noteEdit->toPlainText().trimmed();
    if (!m_ownerEdit->text().trimmed().isEmpty()) values["owner_name"] = m_ownerEdit->text().trimmed();
    if (!m_db->updateRecordById("shared_followup_threads", id, values)) {
        QMessageBox::warning(this, "Update failed", "The selected shared thread could not be updated.");
        return;
    }
    refreshBoard();
}

void SharedNotesFollowUpPage::markSelectedClosed() {
    if (!m_db) return;
    const int row = m_threadsTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Select a thread", "Select a shared follow-up thread first.");
        return;
    }
    const int id = m_threadsTable->item(row, 0)->text().toInt();
    QMap<QString, QString> values;
    values["status"] = "Closed";
    values["last_touched"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    if (!m_followupEdit->toPlainText().trimmed().isEmpty()) values["followup_text"] = m_followupEdit->toPlainText().trimmed();
    if (!m_noteEdit->toPlainText().trimmed().isEmpty()) values["note_text"] = m_noteEdit->toPlainText().trimmed();
    if (!m_db->updateRecordById("shared_followup_threads", id, values)) {
        QMessageBox::warning(this, "Update failed", "The selected shared thread could not be closed.");
        return;
    }
    refreshBoard();
}

void SharedNotesFollowUpPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_threadsTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Select a thread", "Select a shared follow-up thread first.");
        return;
    }
    const int id = m_threadsTable->item(row, 0)->text().toInt();
    if (!m_db->deleteRecordById("shared_followup_threads", id)) {
        QMessageBox::warning(this, "Delete failed", "The selected shared follow-up thread could not be deleted.");
        return;
    }
    refreshBoard();
}
