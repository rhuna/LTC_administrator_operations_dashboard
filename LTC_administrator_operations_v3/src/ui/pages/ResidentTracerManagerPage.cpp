#include "ResidentTracerManagerPage.h"
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

ResidentTracerManagerPage::ResidentTracerManagerPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Resident Tracer Manager", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v78 adds a resident-centered tracer board so leadership can follow active tracers, see which departments are being touched, flag high-risk findings, and capture immediate follow-up action during survey preparation or live survey activity.",
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
    makeStat("Active tracers", m_activeLabel, "Tracer items still open, in progress, awaiting follow-up, or escalated.");
    makeStat("High-risk tracers", m_highRiskLabel, "Tracer items marked high or critical risk and needing leadership attention.");
    makeStat("Needs follow-up", m_followupLabel, "Tracer items still carrying follow-up work that has not been closed out.");
    makeStat("Resolved", m_resolvedLabel, "Tracer items already resolved or closed.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Log tracer", this);
    auto* formLayout = new QFormLayout(formCard);
    m_tracerDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_residentEdit = new QLineEdit(formCard);
    m_roomEdit = new QLineEdit(formCard);
    m_tracerTypeEdit = new QLineEdit(formCard);
    m_focusAreaEdit = new QLineEdit(formCard);
    m_departmentEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_noteEdit = new QTextEdit(formCard);
    m_followupEdit = new QTextEdit(formCard);
    m_noteEdit->setPlaceholderText("What the tracer found or what surveyors focused on.");
    m_followupEdit->setPlaceholderText("Immediate correction, coaching, document pull, or cross-department follow-up.");
    m_noteEdit->setFixedHeight(72);
    m_followupEdit->setFixedHeight(72);

    m_riskCombo = new QComboBox(formCard);
    m_riskCombo->addItems({"Low", "Moderate", "High", "Critical"});

    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Open", "In Progress", "Needs Follow-Up", "Escalated", "Resolved", "Closed"});

    formLayout->addRow("Tracer date", m_tracerDateEdit);
    formLayout->addRow("Resident", m_residentEdit);
    formLayout->addRow("Room", m_roomEdit);
    formLayout->addRow("Tracer type", m_tracerTypeEdit);
    formLayout->addRow("Focus area", m_focusAreaEdit);
    formLayout->addRow("Department", m_departmentEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Risk", m_riskCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Tracer note", m_noteEdit);
    formLayout->addRow("Follow-up action", m_followupEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save tracer", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Resident tracer board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Date", "Resident", "Room", "Tracer Type", "Focus Area", "Department", "Risk", "Status"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_resolvedButton = new QPushButton("Mark Resolved", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_resolvedButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &ResidentTracerManagerPage::addTracer);
    QObject::connect(m_resolvedButton, &QPushButton::clicked, this, &ResidentTracerManagerPage::markSelectedResolved);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &ResidentTracerManagerPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &ResidentTracerManagerPage::refreshBoard);

    refreshBoard();
}

void ResidentTracerManagerPage::addTracer() {
    if (!m_db) return;
    const QString resident = m_residentEdit->text().trimmed();
    const QString tracerType = m_tracerTypeEdit->text().trimmed();
    const QString focusArea = m_focusAreaEdit->text().trimmed();
    if (resident.isEmpty() || tracerType.isEmpty() || focusArea.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Resident, tracer type, and focus area are required.");
        return;
    }

    if (!m_db->addRecord("resident_tracer_items", {
            {"tracer_date", m_tracerDateEdit->text().trimmed()},
            {"resident_name", resident},
            {"room_number", m_roomEdit->text().trimmed()},
            {"tracer_type", tracerType},
            {"focus_area", focusArea},
            {"department_name", m_departmentEdit->text().trimmed()},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"risk_level", m_riskCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"tracer_note", m_noteEdit->toPlainText().trimmed()},
            {"followup_action", m_followupEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The resident tracer item could not be saved.");
        return;
    }

    m_residentEdit->clear();
    m_roomEdit->clear();
    m_tracerTypeEdit->clear();
    m_focusAreaEdit->clear();
    m_departmentEdit->clear();
    m_ownerEdit->clear();
    m_riskCombo->setCurrentText("Moderate");
    m_statusCombo->setCurrentText("Open");
    m_noteEdit->clear();
    m_followupEdit->clear();
    refreshBoard();
}

void ResidentTracerManagerPage::markSelectedResolved() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a tracer item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("resident_tracer_items", id, {{"status", "Resolved"}});
    refreshBoard();
}

void ResidentTracerManagerPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a tracer item first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this resident tracer item?") == QMessageBox::Yes) {
        m_db->deleteRecordById("resident_tracer_items", id);
        refreshBoard();
    }
}

void ResidentTracerManagerPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "resident_tracer_items",
        {"id", "tracer_date", "resident_name", "room_number", "tracer_type", "focus_area", "department_name", "owner_name", "risk_level", "status", "tracer_note", "followup_action"});

    const QString activeFilter = "status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated'";
    const int activeCount = m_db->countWhere("resident_tracer_items", activeFilter);
    const int highRiskCount = m_db->countWhere("resident_tracer_items", QString("(%1) AND (risk_level='High' OR risk_level='Critical')").arg(activeFilter));
    const int followupCount = m_db->countWhere("resident_tracer_items", "status='Needs Follow-Up' OR status='Escalated'");
    const int resolvedCount = m_db->countWhere("resident_tracer_items", "status='Resolved' OR status='Closed'");

    m_activeLabel->setText(QString::number(activeCount));
    m_highRiskLabel->setText(QString::number(highRiskCount));
    m_followupLabel->setText(QString::number(followupCount));
    m_resolvedLabel->setText(QString::number(resolvedCount));

    m_summaryLabel->setText(
        QString("%1 active tracer(s)  ·  %2 high-risk tracer(s)  ·  %3 tracer(s) still needing follow-up  ·  %4 resolved  ·  %5 live request(s) and %6 document pull(s) remain active alongside tracers")
            .arg(activeCount)
            .arg(highRiskCount)
            .arg(followupCount)
            .arg(resolvedCount)
            .arg(m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'"))
            .arg(m_db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("tracer_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* residentItem = new QTableWidgetItem(row.value("resident_name"));
        auto* roomItem = new QTableWidgetItem(row.value("room_number"));
        auto* typeItem = new QTableWidgetItem(row.value("tracer_type"));
        auto* focusItem = new QTableWidgetItem(row.value("focus_area"));
        auto* deptItem = new QTableWidgetItem(row.value("department_name"));
        auto* riskItem = new QTableWidgetItem(row.value("risk_level"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString risk = row.value("risk_level");
        const QString status = row.value("status");
        if (risk == "Critical") riskItem->setForeground(QColor("#b91c1c"));
        else if (risk == "High") riskItem->setForeground(QColor("#9a3412"));
        else if (risk == "Moderate") riskItem->setForeground(QColor("#92400e"));

        if (status == "Resolved" || status == "Closed") statusItem->setForeground(QColor("#166534"));
        else if (status == "Escalated") statusItem->setForeground(QColor("#b91c1c"));
        else if (status == "Needs Follow-Up") statusItem->setForeground(QColor("#92400e"));

        QString tip = row.value("tracer_note").trimmed();
        const QString followup = row.value("followup_action").trimmed();
        if (!followup.isEmpty()) {
            if (!tip.isEmpty()) tip += "\n";
            tip += "Follow-up: " + followup;
        }
        if (!tip.isEmpty()) focusItem->setToolTip(tip);

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, residentItem);
        m_table->setItem(r, 2, roomItem);
        m_table->setItem(r, 3, typeItem);
        m_table->setItem(r, 4, focusItem);
        m_table->setItem(r, 5, deptItem);
        m_table->setItem(r, 6, riskItem);
        m_table->setItem(r, 7, statusItem);
    }
}
