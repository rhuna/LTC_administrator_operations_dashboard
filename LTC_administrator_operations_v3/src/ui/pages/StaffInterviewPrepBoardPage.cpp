#include "StaffInterviewPrepBoardPage.h"
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

StaffInterviewPrepBoardPage::StaffInterviewPrepBoardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Staff Interview Prep Board", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v78 adds a focused staff interview prep board so leadership can coach charge nurses, CNAs, department heads, and support staff on the survey topics they are most likely to be asked about, who still needs coaching, and who is ready for interview rounds.",
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
    makeStat("Open coaching items", m_openLabel, "Interview prep rows still open, assigned, or actively coaching.");
    makeStat("Due today", m_dueTodayLabel, "People leadership should coach or verify today.");
    makeStat("At-risk interviews", m_atRiskLabel, "Staff marked not ready or at risk for interview confidence.");
    makeStat("Ready staff", m_readyLabel, "Rows already marked ready for survey interaction.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Add interview prep item", this);
    auto* formLayout = new QFormLayout(formCard);
    m_interviewDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_staffNameEdit = new QLineEdit(formCard);
    m_roleEdit = new QLineEdit(formCard);
    m_departmentEdit = new QLineEdit(formCard);
    m_focusAreaEdit = new QLineEdit(formCard);
    m_coachEdit = new QLineEdit(formCard);
    m_dueDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_noteEdit = new QTextEdit(formCard);
    m_noteEdit->setPlaceholderText("Coaching points, likely survey questions, confidence gaps, or role-specific reminders.");
    m_noteEdit->setFixedHeight(84);

    m_readinessCombo = new QComboBox(formCard);
    m_readinessCombo->addItems({"Needs Coaching", "Partially Ready", "Ready", "At Risk"});

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "Assigned", "Coaching", "Ready", "Complete"});

    formLayout->addRow("Interview / review date", m_interviewDateEdit);
    formLayout->addRow("Staff member", m_staffNameEdit);
    formLayout->addRow("Role", m_roleEdit);
    formLayout->addRow("Department", m_departmentEdit);
    formLayout->addRow("Focus area", m_focusAreaEdit);
    formLayout->addRow("Coach / owner", m_coachEdit);
    formLayout->addRow("Due date", m_dueDateEdit);
    formLayout->addRow("Readiness", m_readinessCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Coaching notes", m_noteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save prep item", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Interview readiness board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Review Date", "Staff", "Role", "Department", "Focus Area", "Coach", "Readiness", "Status"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_readyButton = new QPushButton("Mark Ready", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_readyButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &StaffInterviewPrepBoardPage::addPrepItem);
    QObject::connect(m_readyButton, &QPushButton::clicked, this, &StaffInterviewPrepBoardPage::markSelectedReady);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &StaffInterviewPrepBoardPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &StaffInterviewPrepBoardPage::refreshBoard);

    refreshBoard();
}

void StaffInterviewPrepBoardPage::addPrepItem() {
    if (!m_db) return;
    const QString staffName = m_staffNameEdit->text().trimmed();
    const QString role = m_roleEdit->text().trimmed();
    const QString focusArea = m_focusAreaEdit->text().trimmed();
    if (staffName.isEmpty() || role.isEmpty() || focusArea.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Staff member, role, and focus area are required.");
        return;
    }

    if (!m_db->addRecord("staff_interview_prep_items", {
            {"review_date", m_interviewDateEdit->text().trimmed()},
            {"staff_name", staffName},
            {"role_name", role},
            {"department_name", m_departmentEdit->text().trimmed()},
            {"focus_area", focusArea},
            {"coach_name", m_coachEdit->text().trimmed()},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"readiness", m_readinessCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"notes", m_noteEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The staff interview prep item could not be saved.");
        return;
    }

    m_staffNameEdit->clear();
    m_roleEdit->clear();
    m_departmentEdit->clear();
    m_focusAreaEdit->clear();
    m_coachEdit->clear();
    m_dueDateEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_readinessCombo->setCurrentText("Needs Coaching");
    m_statusCombo->setCurrentText("Open");
    m_noteEdit->clear();
    refreshBoard();
}

void StaffInterviewPrepBoardPage::markSelectedReady() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a prep item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("staff_interview_prep_items", id, {{"readiness", "Ready"}, {"status", "Ready"}});
    refreshBoard();
}

void StaffInterviewPrepBoardPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a prep item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this staff interview prep item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("staff_interview_prep_items", id);
        refreshBoard();
    }
}

void StaffInterviewPrepBoardPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "staff_interview_prep_items",
        {"id", "review_date", "staff_name", "role_name", "department_name", "focus_area", "coach_name", "due_date", "readiness", "status", "notes"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const QString openFilter = "status='Open' OR status='Assigned' OR status='Coaching'";
    const int openCount = m_db->countWhere("staff_interview_prep_items", openFilter);
    const int dueTodayCount = m_db->countWhere("staff_interview_prep_items", QString("(%1) AND due_date <= '%2'").arg(openFilter, today));
    const int atRiskCount = m_db->countWhere("staff_interview_prep_items", "readiness='Needs Coaching' OR readiness='At Risk'");
    const int readyCount = m_db->countWhere("staff_interview_prep_items", "readiness='Ready' OR status='Ready' OR status='Complete'");

    m_openLabel->setText(QString::number(openCount));
    m_dueTodayLabel->setText(QString::number(dueTodayCount));
    m_atRiskLabel->setText(QString::number(atRiskCount));
    m_readyLabel->setText(QString::number(readyCount));

    m_summaryLabel->setText(
        QString("%1 open coaching item(s)  ·  %2 due today  ·  %3 at-risk interview(s)  ·  %4 ready staff  ·  %5 document-pull item(s) still active alongside interview prep")
            .arg(openCount)
            .arg(dueTodayCount)
            .arg(atRiskCount)
            .arg(readyCount)
            .arg(m_db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("review_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* staffItem = new QTableWidgetItem(row.value("staff_name"));
        auto* roleItem = new QTableWidgetItem(row.value("role_name"));
        auto* departmentItem = new QTableWidgetItem(row.value("department_name"));
        auto* focusItem = new QTableWidgetItem(row.value("focus_area"));
        auto* coachItem = new QTableWidgetItem(row.value("coach_name"));
        auto* readinessItem = new QTableWidgetItem(row.value("readiness"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString readiness = row.value("readiness");
        const QString status = row.value("status");
        if (readiness == "At Risk") readinessItem->setForeground(QColor("#b91c1c"));
        else if (readiness == "Needs Coaching") readinessItem->setForeground(QColor("#92400e"));
        else if (readiness == "Ready") readinessItem->setForeground(QColor("#166534"));

        if (status == "Ready" || status == "Complete") statusItem->setForeground(QColor("#166534"));
        else if (status == "Coaching") statusItem->setForeground(QColor("#0b4f8a"));

        const QString notes = row.value("notes").trimmed();
        if (!notes.isEmpty()) focusItem->setToolTip(notes);

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, staffItem);
        m_table->setItem(r, 2, roleItem);
        m_table->setItem(r, 3, departmentItem);
        m_table->setItem(r, 4, focusItem);
        m_table->setItem(r, 5, coachItem);
        m_table->setItem(r, 6, readinessItem);
        m_table->setItem(r, 7, statusItem);
    }
}
