#include "PlanOfCorrectionBuilderPage.h"
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

PlanOfCorrectionBuilderPage::PlanOfCorrectionBuilderPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Plan of Correction Builder", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v79 adds a plan-of-correction workspace so leadership can convert survey findings, tracer issues, and document gaps into owned corrective actions with root cause, evidence, due dates, and completion tracking.",
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
    makeStat("Open POC items", m_openLabel, "Corrective-action items that are still open, underway, awaiting evidence, or under review.");
    makeStat("Overdue", m_overdueLabel, "POC items whose due date has passed while still incomplete.");
    makeStat("Awaiting evidence", m_evidenceLabel, "Items that need proof of correction before they can be closed.");
    makeStat("Completed", m_completeLabel, "POC items already completed or submitted.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Build corrective action", this);
    auto* formLayout = new QFormLayout(formCard);
    m_findingDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_tagEdit = new QLineEdit(formCard);
    m_areaEdit = new QLineEdit(formCard);
    m_findingEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_dueDateEdit = new QLineEdit(QDate::currentDate().addDays(7).toString("yyyy-MM-dd"), formCard);
    m_rootCauseEdit = new QTextEdit(formCard);
    m_actionEdit = new QTextEdit(formCard);
    m_evidenceEdit = new QTextEdit(formCard);
    m_rootCauseEdit->setPlaceholderText("Why did this happen? What process or systems issue drove the finding?");
    m_actionEdit->setPlaceholderText("Describe the corrective action, re-education, auditing, monitoring, or process fix.");
    m_evidenceEdit->setPlaceholderText("What evidence will prove correction? Audit sheets, policy revision, re-education logs, observation results, etc.");
    m_rootCauseEdit->setFixedHeight(68);
    m_actionEdit->setFixedHeight(68);
    m_evidenceEdit->setFixedHeight(68);

    m_severityCombo = new QComboBox(formCard);
    m_severityCombo->addItems({"Low", "Moderate", "High", "Immediate Jeopardy"});

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "In Progress", "Awaiting Evidence", "Under Review", "Complete", "Submitted"});

    formLayout->addRow("Finding date", m_findingDateEdit);
    formLayout->addRow("Tag / citation", m_tagEdit);
    formLayout->addRow("Focus area", m_areaEdit);
    formLayout->addRow("Finding", m_findingEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Due date", m_dueDateEdit);
    formLayout->addRow("Severity", m_severityCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Root cause", m_rootCauseEdit);
    formLayout->addRow("Corrective action", m_actionEdit);
    formLayout->addRow("Evidence / monitoring", m_evidenceEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save POC item", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Plan-of-correction board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Finding Date", "Tag", "Area", "Finding", "Owner", "Due Date", "Severity", "Status"
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

    QObject::connect(m_addButton, &QPushButton::clicked, this, &PlanOfCorrectionBuilderPage::addItem);
    QObject::connect(m_completeButton, &QPushButton::clicked, this, &PlanOfCorrectionBuilderPage::markSelectedComplete);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &PlanOfCorrectionBuilderPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &PlanOfCorrectionBuilderPage::refreshBoard);

    refreshBoard();
}

void PlanOfCorrectionBuilderPage::addItem() {
    if (!m_db) return;
    const QString area = m_areaEdit->text().trimmed();
    const QString finding = m_findingEdit->text().trimmed();
    const QString owner = m_ownerEdit->text().trimmed();
    if (area.isEmpty() || finding.isEmpty() || owner.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Focus area, finding, and owner are required.");
        return;
    }

    if (!m_db->addRecord("plan_of_correction_items", {
            {"finding_date", m_findingDateEdit->text().trimmed()},
            {"citation_tag", m_tagEdit->text().trimmed()},
            {"focus_area", area},
            {"finding_text", finding},
            {"owner_name", owner},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"severity", m_severityCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"root_cause", m_rootCauseEdit->toPlainText().trimmed()},
            {"corrective_action", m_actionEdit->toPlainText().trimmed()},
            {"evidence_plan", m_evidenceEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The plan-of-correction item could not be saved.");
        return;
    }

    m_tagEdit->clear();
    m_areaEdit->clear();
    m_findingEdit->clear();
    m_ownerEdit->clear();
    m_dueDateEdit->setText(QDate::currentDate().addDays(7).toString("yyyy-MM-dd"));
    m_severityCombo->setCurrentText("Moderate");
    m_statusCombo->setCurrentText("Open");
    m_rootCauseEdit->clear();
    m_actionEdit->clear();
    m_evidenceEdit->clear();
    refreshBoard();
}

void PlanOfCorrectionBuilderPage::markSelectedComplete() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a corrective-action item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("plan_of_correction_items", id, {{"status", "Complete"}});
    refreshBoard();
}

void PlanOfCorrectionBuilderPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a corrective-action item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this plan-of-correction item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("plan_of_correction_items", id);
        refreshBoard();
    }
}

void PlanOfCorrectionBuilderPage::refreshBoard() {
    if (!m_db) return;
    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const auto rows = m_db->fetchTable(
        "plan_of_correction_items",
        {"id", "finding_date", "citation_tag", "focus_area", "finding_text", "owner_name", "due_date", "severity", "status", "root_cause", "corrective_action", "evidence_plan"});

    const QString openFilter = "status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review'";
    const int openCount = m_db->countWhere("plan_of_correction_items", openFilter);
    const int overdueCount = m_db->countWhere("plan_of_correction_items", QString("(%1) AND due_date < '%2'").arg(openFilter, today));
    const int evidenceCount = m_db->countWhere("plan_of_correction_items", "status='Awaiting Evidence' OR status='Under Review'");
    const int completeCount = m_db->countWhere("plan_of_correction_items", "status='Complete' OR status='Submitted'");

    m_openLabel->setText(QString::number(openCount));
    m_overdueLabel->setText(QString::number(overdueCount));
    m_evidenceLabel->setText(QString::number(evidenceCount));
    m_completeLabel->setText(QString::number(completeCount));

    m_summaryLabel->setText(
        QString("%1 open POC item(s)  ·  %2 overdue  ·  %3 awaiting evidence or review  ·  %4 complete/submitted  ·  %5 live requests and %6 tracer items still feed corrective-action pressure")
            .arg(openCount)
            .arg(overdueCount)
            .arg(evidenceCount)
            .arg(completeCount)
            .arg(m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'"))
            .arg(m_db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("finding_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* tagItem = new QTableWidgetItem(row.value("citation_tag"));
        auto* areaItem = new QTableWidgetItem(row.value("focus_area"));
        auto* findingItem = new QTableWidgetItem(row.value("finding_text"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* dueItem = new QTableWidgetItem(row.value("due_date"));
        auto* severityItem = new QTableWidgetItem(row.value("severity"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString severity = row.value("severity");
        const QString status = row.value("status");
        if (severity == "Immediate Jeopardy") severityItem->setForeground(QColor("#991b1b"));
        else if (severity == "High") severityItem->setForeground(QColor("#9a3412"));
        else if (severity == "Moderate") severityItem->setForeground(QColor("#92400e"));

        if (status == "Complete" || status == "Submitted") statusItem->setForeground(QColor("#166534"));
        else if (status == "Awaiting Evidence" || status == "Under Review") statusItem->setForeground(QColor("#92400e"));
        else if (status == "Open") statusItem->setForeground(QColor("#0f4c81"));

        QString tip = row.value("root_cause").trimmed();
        const QString action = row.value("corrective_action").trimmed();
        const QString evidence = row.value("evidence_plan").trimmed();
        if (!action.isEmpty()) {
            if (!tip.isEmpty()) tip += "
";
            tip += "Action: " + action;
        }
        if (!evidence.isEmpty()) {
            if (!tip.isEmpty()) tip += "
";
            tip += "Evidence: " + evidence;
        }
        if (!tip.isEmpty()) findingItem->setToolTip(tip);

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, tagItem);
        m_table->setItem(r, 2, areaItem);
        m_table->setItem(r, 3, findingItem);
        m_table->setItem(r, 4, ownerItem);
        m_table->setItem(r, 5, dueItem);
        m_table->setItem(r, 6, severityItem);
        m_table->setItem(r, 7, statusItem);
    }
}
