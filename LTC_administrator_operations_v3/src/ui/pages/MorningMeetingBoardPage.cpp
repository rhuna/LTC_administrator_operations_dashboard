#include "MorningMeetingBoardPage.h"
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

MorningMeetingBoardPage::MorningMeetingBoardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Morning Meeting Board", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v68 adds a simple daily-priority board so leadership can convert rounds and executive follow-up into the handful of actions that matter today: who owns them, what time they are due, and what is blocked.",
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
    makeStat("Active today", m_activeLabel, "Open, in-progress, or blocked items on the day board.");
    makeStat("Due now", m_todayLabel, "Items due today that are still not closed.");
    makeStat("Blocked", m_blockedLabel, "Work that needs escalation or barrier removal.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Add daily priority item", this);
    auto* formLayout = new QFormLayout(formCard);

    m_dateEdit = new QLineEdit(formCard);
    m_departmentEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_dueTimeEdit = new QLineEdit(formCard);
    m_priorityCombo = new QComboBox(formCard);
    m_statusCombo = new QComboBox(formCard);
    m_itemEdit = new QTextEdit(formCard);

    m_dateEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_departmentEdit->setPlaceholderText("Staffing / Admissions / DON / EVS / Dietary / MDS");
    m_ownerEdit->setPlaceholderText("Administrator / DON / Department Head");
    m_dueTimeEdit->setText("10:00");
    m_priorityCombo->addItems({"Critical", "High", "Medium", "Low"});
    m_statusCombo->addItems({"Open", "In Progress", "Blocked", "Done", "Closed"});
    m_itemEdit->setPlaceholderText("Describe the most important thing that must move forward today.");
    m_itemEdit->setMinimumHeight(84);

    formLayout->addRow("Board date:", m_dateEdit);
    formLayout->addRow("Department:", m_departmentEdit);
    formLayout->addRow("Owner:", m_ownerEdit);
    formLayout->addRow("Due time:", m_dueTimeEdit);
    formLayout->addRow("Priority:", m_priorityCombo);
    formLayout->addRow("Status:", m_statusCombo);
    formLayout->addRow("Priority item:", m_itemEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save priority item", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Today's leadership board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Board Date", "Department", "Priority Item", "Owner", "Due Time", "Status"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_doneButton = new QPushButton("Mark Done", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_doneButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &MorningMeetingBoardPage::addPriorityItem);
    QObject::connect(m_doneButton, &QPushButton::clicked, this, &MorningMeetingBoardPage::markSelectedDone);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &MorningMeetingBoardPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &MorningMeetingBoardPage::refreshBoard);

    refreshBoard();
}

void MorningMeetingBoardPage::addPriorityItem() {
    if (!m_db) return;

    const QString itemName = m_itemEdit->toPlainText().trimmed();
    const QString department = m_departmentEdit->text().trimmed();
    if (itemName.isEmpty() || department.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Department and priority item are required.");
        return;
    }

    if (!m_db->addRecord("morning_meeting_items", {
            {"board_date", m_dateEdit->text().trimmed()},
            {"department_name", department},
            {"item_name", itemName},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"due_time", m_dueTimeEdit->text().trimmed()},
            {"priority", m_priorityCombo->currentText()},
            {"status", m_statusCombo->currentText()}
        })) {
        QMessageBox::warning(this, "Save failed", "The morning meeting item could not be saved.");
        return;
    }

    m_departmentEdit->clear();
    m_ownerEdit->clear();
    m_dueTimeEdit->setText("10:00");
    m_priorityCombo->setCurrentText("High");
    m_statusCombo->setCurrentText("Open");
    m_itemEdit->clear();
    refreshBoard();
}

void MorningMeetingBoardPage::markSelectedDone() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a board item first.");
        return;
    }

    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;

    m_db->updateRecordById("morning_meeting_items", id, {{"status", "Done"}});
    refreshBoard();
}

void MorningMeetingBoardPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a board item first.");
        return;
    }

    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;

    if (QMessageBox::question(this, "Confirm delete", "Delete this morning meeting item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("morning_meeting_items", id);
        refreshBoard();
    }
}

void MorningMeetingBoardPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "morning_meeting_items",
        {"id", "board_date", "department_name", "item_name", "owner_name", "due_time", "priority", "status"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const int activeCount = m_db->countWhere("morning_meeting_items", "status='Open' OR status='In Progress' OR status='Blocked'");
    const int dueNowCount = m_db->countWhere("morning_meeting_items",
        QString("board_date <= '%1' AND (status='Open' OR status='In Progress' OR status='Blocked')").arg(today));
    const int blockedCount = m_db->countWhere("morning_meeting_items", "status='Blocked'");

    m_activeLabel->setText(QString::number(activeCount));
    m_todayLabel->setText(QString::number(dueNowCount));
    m_blockedLabel->setText(QString::number(blockedCount));

    m_summaryLabel->setText(
        QString("%1 active board item(s)  ·  %2 due today or earlier  ·  %3 blocked item(s)  ·  %4 executive follow-up item(s) still active behind the scenes")
            .arg(activeCount)
            .arg(dueNowCount)
            .arg(blockedCount)
            .arg(m_db->countWhere("executive_followups", "status='Open' OR status='In Progress' OR status='Watch' OR status='Blocked'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("board_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        const QString priority = row.value("priority");
        const QString status = row.value("status");

        if ((status == "Open" || status == "In Progress" || status == "Blocked") && row.value("board_date") <= today) {
            dateItem->setForeground(QColor("#b91c1c"));
        }

        auto* departmentItem = new QTableWidgetItem(row.value("department_name"));
        auto* nameItem = new QTableWidgetItem(row.value("item_name"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* dueItem = new QTableWidgetItem(row.value("due_time"));
        auto* statusItem = new QTableWidgetItem(status);

        if (priority == "Critical" || priority == "High") nameItem->setForeground(QColor("#92400e"));
        if (status == "Done" || status == "Closed") statusItem->setForeground(QColor("#166534"));
        else if (status == "Blocked") statusItem->setForeground(QColor("#b91c1c"));
        else if (status == "In Progress") statusItem->setForeground(QColor("#0b4f8a"));

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, departmentItem);
        m_table->setItem(r, 2, nameItem);
        m_table->setItem(r, 3, ownerItem);
        m_table->setItem(r, 4, dueItem);
        m_table->setItem(r, 5, statusItem);
    }
}
