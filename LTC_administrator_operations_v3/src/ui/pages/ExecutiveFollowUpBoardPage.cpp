#include "ExecutiveFollowUpBoardPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QColor>
#include <QComboBox>
#include <QDate>
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

ExecutiveFollowUpBoardPage::ExecutiveFollowUpBoardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Executive Follow-Up Board", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v67 builds on Leadership Rounds by giving leadership one place to track the follow-up work that comes out of morning rounds, huddle decisions, and executive escalation items.",
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
        valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #102a43;");
        auto* hint = new QLabel(hintText, card);
        hint->setStyleSheet("color: #52606d;");
        hint->setWordWrap(true);
        layout->addWidget(valueLabel);
        layout->addWidget(hint);
        strip->addWidget(card);
    };
    makeStat("Open follow-up", m_openLabel, "Items still open, in progress, or on watch.");
    makeStat("Due today / overdue", m_todayLabel, "Immediate same-day leadership attention.");
    makeStat("High priority", m_highLabel, "High and urgent commitments still active.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Add executive follow-up item", this);
    auto* formLayout = new QFormLayout(formCard);

    m_dueDateEdit = new QLineEdit(formCard);
    m_focusAreaEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_sourceEdit = new QLineEdit(formCard);
    m_priorityCombo = new QComboBox(formCard);
    m_statusCombo = new QComboBox(formCard);
    m_itemEdit = new QTextEdit(formCard);

    m_dueDateEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_focusAreaEdit->setPlaceholderText("Staffing / Admissions / Dining / Survey / Clinical / EVS");
    m_ownerEdit->setPlaceholderText("Administrator / DON / Department Head / Admissions");
    m_sourceEdit->setText("Leadership Rounds");
    m_priorityCombo->addItems({"High", "Urgent", "Medium", "Low"});
    m_statusCombo->addItems({"Open", "In Progress", "Watch", "Blocked", "Complete", "Closed"});
    m_itemEdit->setPlaceholderText("Describe the specific leadership follow-up item, barrier, or decision that needs to be tracked to closure.");
    m_itemEdit->setMinimumHeight(90);

    formLayout->addRow("Due date:", m_dueDateEdit);
    formLayout->addRow("Focus area:", m_focusAreaEdit);
    formLayout->addRow("Owner:", m_ownerEdit);
    formLayout->addRow("Source:", m_sourceEdit);
    formLayout->addRow("Priority:", m_priorityCombo);
    formLayout->addRow("Status:", m_statusCombo);
    formLayout->addRow("Follow-up item:", m_itemEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save follow-up item", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Leadership follow-up board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Due Date", "Focus Area", "Follow-Up Item", "Owner", "Priority", "Status"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_completeButton = new QPushButton("Mark Complete", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_completeButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &ExecutiveFollowUpBoardPage::addFollowUp);
    QObject::connect(m_completeButton, &QPushButton::clicked, this, &ExecutiveFollowUpBoardPage::markSelectedComplete);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &ExecutiveFollowUpBoardPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &ExecutiveFollowUpBoardPage::refreshBoard);

    refreshBoard();
}

void ExecutiveFollowUpBoardPage::addFollowUp() {
    if (!m_db) return;

    const QString itemName = m_itemEdit->toPlainText().trimmed();
    const QString focusArea = m_focusAreaEdit->text().trimmed();
    if (itemName.isEmpty() || focusArea.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Focus area and follow-up item are required.");
        return;
    }

    if (!m_db->addRecord("executive_followups", {
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"focus_area", focusArea},
            {"item_name", itemName},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"priority", m_priorityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"source_name", m_sourceEdit->text().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The executive follow-up item could not be saved.");
        return;
    }

    m_focusAreaEdit->clear();
    m_ownerEdit->clear();
    m_sourceEdit->setText("Leadership Rounds");
    m_priorityCombo->setCurrentText("High");
    m_statusCombo->setCurrentText("Open");
    m_itemEdit->clear();
    refreshBoard();
}

void ExecutiveFollowUpBoardPage::markSelectedComplete() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a follow-up item first.");
        return;
    }

    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;

    m_db->updateRecordById("executive_followups", id, {{"status", "Complete"}});
    refreshBoard();
}

void ExecutiveFollowUpBoardPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a follow-up item first.");
        return;
    }

    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;

    if (QMessageBox::question(this, "Confirm delete", "Delete this executive follow-up item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("executive_followups", id);
        refreshBoard();
    }
}

void ExecutiveFollowUpBoardPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "executive_followups",
        {"id", "due_date", "focus_area", "item_name", "owner_name", "priority", "status", "source_name"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const int openCount = m_db->countWhere("executive_followups", "status='Open' OR status='In Progress' OR status='Watch' OR status='Blocked'");
    const int dueToday = m_db->countWhere("executive_followups",
        QString("due_date <= '%1' AND (status='Open' OR status='In Progress' OR status='Watch' OR status='Blocked')").arg(today));
    const int highCount = m_db->countWhere("executive_followups",
        "(priority='High' OR priority='Urgent') AND (status='Open' OR status='In Progress' OR status='Watch' OR status='Blocked')");

    m_openLabel->setText(QString::number(openCount));
    m_todayLabel->setText(QString::number(dueToday));
    m_highLabel->setText(QString::number(highCount));

    m_summaryLabel->setText(
        QString("%1 active follow-up item(s)  ·  %2 due today or overdue  ·  %3 high-priority item(s)  ·  %4 leadership rounds note(s) still open")
            .arg(openCount)
            .arg(dueToday)
            .arg(highCount)
            .arg(m_db->countWhere("leadership_rounds", "status='Open' OR status='In Progress' OR status='Watch'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dueItem = new QTableWidgetItem(row.value("due_date"));
        dueItem->setData(Qt::UserRole, row.value("id").toInt());
        const QString priority = row.value("priority");
        const QString status = row.value("status");

        if ((status == "Open" || status == "In Progress" || status == "Watch" || status == "Blocked") && row.value("due_date") <= today) {
            dueItem->setForeground(QColor("#b91c1c"));
        }

        auto* focusItem = new QTableWidgetItem(row.value("focus_area"));
        auto* nameItem = new QTableWidgetItem(row.value("item_name"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* priorityItem = new QTableWidgetItem(priority);
        auto* statusItem = new QTableWidgetItem(status);

        if (priority == "Urgent" || priority == "High") priorityItem->setForeground(QColor("#92400e"));
        if (status == "Complete" || status == "Closed") statusItem->setForeground(QColor("#166534"));
        else if (status == "Blocked") statusItem->setForeground(QColor("#b91c1c"));
        else if (status == "Watch") statusItem->setForeground(QColor("#92400e"));

        m_table->setItem(r, 0, dueItem);
        m_table->setItem(r, 1, focusItem);
        m_table->setItem(r, 2, nameItem);
        m_table->setItem(r, 3, ownerItem);
        m_table->setItem(r, 4, priorityItem);
        m_table->setItem(r, 5, statusItem);
    }
}
