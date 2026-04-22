#include "MockSurveyDrillBoardPage.h"
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

MockSurveyDrillBoardPage::MockSurveyDrillBoardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Mock Survey Drill Board", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v73 adds a mock survey drill board so leadership can assign interview prep, tracer walks, dining observations, and survey drills with clear ownership and same-week follow-through.",
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
    makeStat("Open drills", m_openLabel, "Mock survey items still open, assigned, or actively being drilled with the team.");
    makeStat("Due now", m_dueLabel, "Open drills due today or already overdue for practice or follow-up.");
    makeStat("Completed", m_completeLabel, "Mock survey drills that were practiced and closed out.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Log mock survey drill item", this);
    auto* formLayout = new QFormLayout(formCard);
    m_drillDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_drillTypeEdit = new QLineEdit(formCard);
    m_areaEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_dueDateEdit = new QLineEdit(QDate::currentDate().addDays(1).toString("yyyy-MM-dd"), formCard);
    m_noteEdit = new QTextEdit(formCard);
    m_noteEdit->setPlaceholderText("Interview cue, tracer focus, observation concern, or coaching note.");
    m_noteEdit->setFixedHeight(84);

    m_priorityCombo = new QComboBox(formCard);
    m_priorityCombo->addItems({"High", "Medium", "Low"});

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Assigned", "In Drill", "Complete"});

    formLayout->addRow("Drill date", m_drillDateEdit);
    formLayout->addRow("Drill type", m_drillTypeEdit);
    formLayout->addRow("Focus area", m_areaEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Due date", m_dueDateEdit);
    formLayout->addRow("Priority", m_priorityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Coaching / note", m_noteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save drill item", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Mock survey drill board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Drill Date", "Drill Type", "Focus Area", "Owner", "Due Date", "Priority", "Status"
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

    QObject::connect(m_addButton, &QPushButton::clicked, this, &MockSurveyDrillBoardPage::addDrillItem);
    QObject::connect(m_completeButton, &QPushButton::clicked, this, &MockSurveyDrillBoardPage::markSelectedComplete);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &MockSurveyDrillBoardPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &MockSurveyDrillBoardPage::refreshBoard);

    refreshBoard();
}

void MockSurveyDrillBoardPage::addDrillItem() {
    if (!m_db) return;
    const QString drillType = m_drillTypeEdit->text().trimmed();
    const QString area = m_areaEdit->text().trimmed();
    if (drillType.isEmpty() || area.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Drill type and focus area are required.");
        return;
    }

    if (!m_db->addRecord("mock_survey_drills", {
            {"drill_date", m_drillDateEdit->text().trimmed()},
            {"drill_type", drillType},
            {"focus_area", area},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"priority", m_priorityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"coaching_note", m_noteEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The mock survey drill item could not be saved.");
        return;
    }

    m_drillTypeEdit->clear();
    m_areaEdit->clear();
    m_ownerEdit->clear();
    m_dueDateEdit->setText(QDate::currentDate().addDays(1).toString("yyyy-MM-dd"));
    m_priorityCombo->setCurrentText("High");
    m_statusCombo->setCurrentText("Open");
    m_noteEdit->clear();
    refreshBoard();
}

void MockSurveyDrillBoardPage::markSelectedComplete() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a drill item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("mock_survey_drills", id, {{"status", "Complete"}});
    refreshBoard();
}

void MockSurveyDrillBoardPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a drill item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this mock survey drill item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("mock_survey_drills", id);
        refreshBoard();
    }
}

void MockSurveyDrillBoardPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "mock_survey_drills",
        {"id", "drill_date", "drill_type", "focus_area", "owner_name", "due_date", "coaching_note", "priority", "status"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const int openCount = m_db->countWhere("mock_survey_drills", "status='Open' OR status='Assigned' OR status='In Drill'");
    const int dueCount = m_db->countWhere("mock_survey_drills", QString("(status='Open' OR status='Assigned' OR status='In Drill') AND due_date <= '%1'").arg(today));
    const int completeCount = m_db->countWhere("mock_survey_drills", "status='Complete'");

    m_openLabel->setText(QString::number(openCount));
    m_dueLabel->setText(QString::number(dueCount));
    m_completeLabel->setText(QString::number(completeCount));
    m_summaryLabel->setText(
        QString("Mock survey drill summary: %1 open item(s), %2 due-now item(s), %3 completed item(s).")
            .arg(openCount).arg(dueCount).arg(completeCount));

    m_table->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows.at(r);
        auto* drillDateItem = new QTableWidgetItem(row.value("drill_date"));
        drillDateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* typeItem = new QTableWidgetItem(row.value("drill_type"));
        auto* areaItem = new QTableWidgetItem(row.value("focus_area"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* dueDateItem = new QTableWidgetItem(row.value("due_date"));
        auto* priorityItem = new QTableWidgetItem(row.value("priority"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString priority = row.value("priority");
        const QString status = row.value("status");
        if (priority == "High") priorityItem->setForeground(QColor("#92400e"));
        else if (priority == "Medium") priorityItem->setForeground(QColor("#0b4f8a"));
        else if (priority == "Low") priorityItem->setForeground(QColor("#166534"));

        if (status == "Complete") statusItem->setForeground(QColor("#166534"));
        else if (status == "In Drill") statusItem->setForeground(QColor("#0b4f8a"));
        else if (status == "Assigned") statusItem->setForeground(QColor("#7c3aed"));
        else if (status == "Open") statusItem->setForeground(QColor("#92400e"));

        const QString note = row.value("coaching_note").trimmed();
        if (!note.isEmpty()) typeItem->setToolTip(note);

        m_table->setItem(r, 0, drillDateItem);
        m_table->setItem(r, 1, typeItem);
        m_table->setItem(r, 2, areaItem);
        m_table->setItem(r, 3, ownerItem);
        m_table->setItem(r, 4, dueDateItem);
        m_table->setItem(r, 5, priorityItem);
        m_table->setItem(r, 6, statusItem);
    }
}
