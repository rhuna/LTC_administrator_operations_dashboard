
#include "SurveyRecoveryBoardPage.h"
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

SurveyRecoveryBoardPage::SurveyRecoveryBoardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Survey Recovery Board", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v71 adds a focused survey recovery board so leadership can move from identified issue to owned corrective action, due-date follow-up, and evidence-ready closure.",
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
    makeStat("Open recovery items", m_openLabel, "Action-plan items still open, in progress, or awaiting evidence.");
    makeStat("Overdue", m_overdueLabel, "Open recovery items whose due date has already passed.");
    makeStat("Completed", m_completeLabel, "Corrective-action items closed with evidence-ready status.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Log corrective action item", this);
    auto* formLayout = new QFormLayout(formCard);
    m_planDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_focusAreaEdit = new QLineEdit(formCard);
    m_issueEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_dueDateEdit = new QLineEdit(QDate::currentDate().addDays(2).toString("yyyy-MM-dd"), formCard);
    m_noteEdit = new QTextEdit(formCard);
    m_noteEdit->setPlaceholderText("Root cause, corrective action, monitoring plan, or evidence note.");
    m_noteEdit->setFixedHeight(84);

    m_priorityCombo = new QComboBox(formCard);
    m_priorityCombo->addItems({"High", "Medium", "Low"});

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "In Progress", "Awaiting Evidence", "Complete"});

    formLayout->addRow("Plan date", m_planDateEdit);
    formLayout->addRow("Focus area", m_focusAreaEdit);
    formLayout->addRow("Issue / corrective action", m_issueEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Due date", m_dueDateEdit);
    formLayout->addRow("Priority", m_priorityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Evidence / note", m_noteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save recovery item", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Recovery tracking board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Plan Date", "Focus Area", "Issue / Action", "Owner", "Due Date", "Priority", "Status"
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

    QObject::connect(m_addButton, &QPushButton::clicked, this, &SurveyRecoveryBoardPage::addRecoveryItem);
    QObject::connect(m_completeButton, &QPushButton::clicked, this, &SurveyRecoveryBoardPage::markSelectedComplete);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &SurveyRecoveryBoardPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &SurveyRecoveryBoardPage::refreshBoard);

    refreshBoard();
}

void SurveyRecoveryBoardPage::addRecoveryItem() {
    if (!m_db) return;
    const QString focusArea = m_focusAreaEdit->text().trimmed();
    const QString issue = m_issueEdit->text().trimmed();
    if (focusArea.isEmpty() || issue.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Focus area and issue / action are required.");
        return;
    }

    if (!m_db->addRecord("survey_recovery_items", {
            {"plan_date", m_planDateEdit->text().trimmed()},
            {"focus_area", focusArea},
            {"issue_name", issue},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"priority", m_priorityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"evidence_note", m_noteEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The survey recovery item could not be saved.");
        return;
    }

    m_focusAreaEdit->clear();
    m_issueEdit->clear();
    m_ownerEdit->clear();
    m_dueDateEdit->setText(QDate::currentDate().addDays(2).toString("yyyy-MM-dd"));
    m_priorityCombo->setCurrentText("High");
    m_statusCombo->setCurrentText("Open");
    m_noteEdit->clear();
    refreshBoard();
}

void SurveyRecoveryBoardPage::markSelectedComplete() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a recovery item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("survey_recovery_items", id, {{"status", "Complete"}});
    refreshBoard();
}

void SurveyRecoveryBoardPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a recovery item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this survey recovery item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("survey_recovery_items", id);
        refreshBoard();
    }
}

void SurveyRecoveryBoardPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "survey_recovery_items",
        {"id", "plan_date", "focus_area", "issue_name", "owner_name", "due_date", "evidence_note", "priority", "status"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const int openCount = m_db->countWhere("survey_recovery_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence'");
    const int overdueCount = m_db->countWhere("survey_recovery_items", QString("(status='Open' OR status='In Progress' OR status='Awaiting Evidence') AND due_date < '%1'").arg(today));
    const int completeCount = m_db->countWhere("survey_recovery_items", "status='Complete'");

    m_openLabel->setText(QString::number(openCount));
    m_overdueLabel->setText(QString::number(overdueCount));
    m_completeLabel->setText(QString::number(completeCount));

    m_summaryLabel->setText(
        QString("%1 open recovery item(s)  ·  %2 overdue  ·  %3 completed  ·  %4 survey-readiness item(s) still remain open on the broader readiness board")
            .arg(openCount)
            .arg(overdueCount)
            .arg(completeCount)
            .arg(m_db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* planDateItem = new QTableWidgetItem(row.value("plan_date"));
        planDateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* focusAreaItem = new QTableWidgetItem(row.value("focus_area"));
        auto* issueItem = new QTableWidgetItem(row.value("issue_name"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* dueDateItem = new QTableWidgetItem(row.value("due_date"));
        auto* priorityItem = new QTableWidgetItem(row.value("priority"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString priority = row.value("priority");
        const QString status = row.value("status");
        if (priority == "High") priorityItem->setForeground(QColor("#92400e"));
        if (status == "Complete") statusItem->setForeground(QColor("#166534"));
        else if (status == "Awaiting Evidence") statusItem->setForeground(QColor("#0b4f8a"));
        else if (status == "Open") statusItem->setForeground(QColor("#92400e"));

        const QString note = row.value("evidence_note").trimmed();
        if (!note.isEmpty()) issueItem->setToolTip(note);

        m_table->setItem(r, 0, planDateItem);
        m_table->setItem(r, 1, focusAreaItem);
        m_table->setItem(r, 2, issueItem);
        m_table->setItem(r, 3, ownerItem);
        m_table->setItem(r, 4, dueDateItem);
        m_table->setItem(r, 5, priorityItem);
        m_table->setItem(r, 6, statusItem);
    }
}
