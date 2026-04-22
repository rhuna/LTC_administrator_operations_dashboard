#include "LeadershipHuddleGeneratorPage.h"
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
#include <QTime>
#include <QVBoxLayout>

LeadershipHuddleGeneratorPage::LeadershipHuddleGeneratorPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Leadership Huddle Generator", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v84 adds a dedicated huddle workspace so leadership can turn live survey data into a practical agenda for the next stand-up, command-center review, or executive touchpoint without manually pulling counts from multiple boards.",
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
    makeStat("Open agendas", m_openLabel, "Huddle agendas still being drafted or actively adjusted before the next leadership touchpoint.");
    makeStat("Due now", m_dueLabel, "Huddles scheduled for today or already due based on the planned date/time.");
    makeStat("Ready", m_readyLabel, "Agendas finalized and ready to print, review, or read from the screen.");
    makeStat("Completed", m_completedLabel, "Huddles already held or closed out after the meeting.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Build leadership huddle agenda", this);
    auto* formLayout = new QFormLayout(formCard);
    m_huddleDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_huddleNameEdit = new QLineEdit("Daily Survey Leadership Huddle", formCard);
    m_ownerEdit = new QLineEdit("Administrator", formCard);
    m_dueTimeEdit = new QLineEdit(QTime::currentTime().addSecs(3600).toString("HH:mm"), formCard);
    m_audienceCombo = new QComboBox(formCard);
    m_audienceCombo->addItems({"Administrator / DON / department heads", "Administrator / survey escort team", "Administrator / clinical leadership", "Executive response team"});
    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Drafting", "Ready", "Completed"});
    m_prioritiesEdit = new QTextEdit(formCard);
    m_prioritiesEdit->setPlaceholderText("Top priorities and huddle talking points will appear here.");
    m_prioritiesEdit->setFixedHeight(120);
    m_notesEdit = new QTextEdit(formCard);
    m_notesEdit->setPlaceholderText("Capture facilitation notes, who should speak first, and what follow-up should happen after the huddle.");
    m_notesEdit->setFixedHeight(84);

    formLayout->addRow("Huddle date", m_huddleDateEdit);
    formLayout->addRow("Huddle name", m_huddleNameEdit);
    formLayout->addRow("Audience", m_audienceCombo);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Due / start time", m_dueTimeEdit);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Top priorities", m_prioritiesEdit);
    formLayout->addRow("Facilitator notes", m_notesEdit);

    auto* buttonRow = new QHBoxLayout();
    m_generateButton = new QPushButton("Generate Agenda", formCard);
    m_addButton = new QPushButton("Save Huddle", formCard);
    buttonRow->addWidget(m_generateButton);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Leadership huddle queue", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Huddle Date", "Huddle", "Audience", "Owner", "Time", "Status"
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
    m_completedButton = new QPushButton("Mark Completed", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_readyButton);
    actionBar->addWidget(m_completedButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_generateButton, &QPushButton::clicked, this, &LeadershipHuddleGeneratorPage::generateAgenda);
    QObject::connect(m_addButton, &QPushButton::clicked, this, &LeadershipHuddleGeneratorPage::addEntry);
    QObject::connect(m_readyButton, &QPushButton::clicked, this, &LeadershipHuddleGeneratorPage::markSelectedReady);
    QObject::connect(m_completedButton, &QPushButton::clicked, this, &LeadershipHuddleGeneratorPage::markSelectedCompleted);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &LeadershipHuddleGeneratorPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &LeadershipHuddleGeneratorPage::refreshBoard);

    generateAgenda();
    refreshBoard();
}

void LeadershipHuddleGeneratorPage::generateAgenda() {
    if (!m_db) return;

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const QString nowTime = QTime::currentTime().toString("HH:mm");

    const int liveOpen = m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'");
    const int liveDue = m_db->countWhere("survey_live_requests", QString("(status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon') AND request_date='%1' AND due_time <= '%2'").arg(today, nowTime));
    const int docsOpen = m_db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'");
    const int docsMissing = m_db->countWhere("survey_document_requests", "status='Missing'");
    const int tracersOpen = m_db->countWhere("resident_tracer_items", "status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated'");
    const int tracerRisk = m_db->countWhere("resident_tracer_items", "(status='Open' OR status='In Progress' OR status='Needs Follow-Up' OR status='Escalated') AND (risk_level='High' OR risk_level='Critical')");
    const int pocOpen = m_db->countWhere("plan_of_correction_items", "status='Open' OR status='In Progress' OR status='Awaiting Evidence' OR status='Under Review'");
    const int pocEvidence = m_db->countWhere("plan_of_correction_items", "status='Awaiting Evidence' OR status='Under Review'");
    const int alertsOpen = m_db->countWhere("alerts_escalation_items", "status='Open' OR status='Due Today' OR status='Blocked'");
    const int alertsCritical = m_db->countWhere("alerts_escalation_items", "(status='Open' OR status='Due Today' OR status='Blocked') AND severity='Critical'");
    const int packetReady = m_db->countWhere("executive_export_packets", "status='Ready'");
    const int barriers = m_db->countWhere("barrier_escalations", "status='Open' OR status='Assigned' OR status='Waiting'");
    const int huddleOpen = m_db->countWhere("morning_meeting_items", "status='Open' OR status='In Progress' OR status='Blocked'");

    const QString agenda = QString(
        "1. Survey urgency review — %1 live request(s) open, %2 due now.\n"
        "2. Document pull status — %3 request(s) active, %4 missing/blocked.\n"
        "3. Resident tracer review — %5 tracer item(s) open, %6 high-risk.\n"
        "4. Plan of correction — %7 item(s) open, %8 awaiting evidence or review.\n"
        "5. Escalations — %9 alert-center item(s) open, %10 critical.\n"
        "6. Barrier removal — %11 barrier item(s) still open.\n"
        "7. Packet / handoff readiness — %12 packet(s) ready for print/export.\n"
        "8. Today's execution board — %13 morning-meeting item(s) still active.\n"
        "9. Owner check-in — confirm who is speaking to each issue and next deadline.\n"
        "10. End huddle with immediate assignments, survey escort expectations, and next check-in time.")
            .arg(liveOpen)
            .arg(liveDue)
            .arg(docsOpen)
            .arg(docsMissing)
            .arg(tracersOpen)
            .arg(tracerRisk)
            .arg(pocOpen)
            .arg(pocEvidence)
            .arg(alertsOpen)
            .arg(alertsCritical)
            .arg(barriers)
            .arg(packetReady)
            .arg(huddleOpen);

    m_prioritiesEdit->setPlainText(agenda);
    if (m_notesEdit->toPlainText().trimmed().isEmpty()) {
        m_notesEdit->setPlainText("Facilitate the huddle from highest-risk issue to lowest-risk issue. Confirm owner, next deliverable, and the time of the next command-center update before closing.");
    }
}

void LeadershipHuddleGeneratorPage::addEntry() {
    if (!m_db) return;
    const QString huddleName = m_huddleNameEdit->text().trimmed();
    const QString owner = m_ownerEdit->text().trimmed();
    const QString priorities = m_prioritiesEdit->toPlainText().trimmed();
    if (huddleName.isEmpty() || owner.isEmpty() || priorities.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Huddle name, owner, and top priorities are required.");
        return;
    }

    if (!m_db->addRecord("leadership_huddle_agendas", {
            {"huddle_date", m_huddleDateEdit->text().trimmed()},
            {"huddle_name", huddleName},
            {"audience_name", m_audienceCombo->currentText()},
            {"owner_name", owner},
            {"due_time", m_dueTimeEdit->text().trimmed()},
            {"status", m_statusCombo->currentText()},
            {"top_priorities", priorities},
            {"facilitator_notes", m_notesEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The huddle agenda could not be saved.");
        return;
    }

    m_huddleNameEdit->setText("Daily Survey Leadership Huddle");
    m_ownerEdit->setText("Administrator");
    m_dueTimeEdit->setText(QTime::currentTime().addSecs(3600).toString("HH:mm"));
    m_statusCombo->setCurrentText("Drafting");
    generateAgenda();
    refreshBoard();
}

void LeadershipHuddleGeneratorPage::markSelectedReady() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a huddle entry first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("leadership_huddle_agendas", id, {{"status", "Ready"}});
    refreshBoard();
}

void LeadershipHuddleGeneratorPage::markSelectedCompleted() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a huddle entry first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("leadership_huddle_agendas", id, {{"status", "Completed"}});
    refreshBoard();
}

void LeadershipHuddleGeneratorPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a huddle entry first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this leadership huddle agenda?") == QMessageBox::Yes) {
        m_db->deleteRecordById("leadership_huddle_agendas", id);
        refreshBoard();
    }
}

void LeadershipHuddleGeneratorPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "leadership_huddle_agendas",
        {"id", "huddle_date", "huddle_name", "audience_name", "owner_name", "due_time", "status", "top_priorities", "facilitator_notes"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const QString nowTime = QTime::currentTime().toString("HH:mm");

    const int openCount = m_db->countWhere("leadership_huddle_agendas", "status='Drafting' OR status='Ready'");
    const int dueCount = m_db->countWhere("leadership_huddle_agendas", QString("(status='Drafting' OR status='Ready') AND (huddle_date < '%1' OR (huddle_date='%1' AND due_time <= '%2'))").arg(today, nowTime));
    const int readyCount = m_db->countWhere("leadership_huddle_agendas", "status='Ready'");
    const int completedCount = m_db->countWhere("leadership_huddle_agendas", "status='Completed'");

    m_openLabel->setText(QString::number(openCount));
    m_dueLabel->setText(QString::number(dueCount));
    m_readyLabel->setText(QString::number(readyCount));
    m_completedLabel->setText(QString::number(completedCount));

    m_summaryLabel->setText(
        QString("%1 huddle agenda(s) open  ·  %2 due now  ·  %3 ready  ·  %4 completed  ·  %5 live request(s), %6 document pull(s), and %7 alert item(s) currently shape the next leadership huddle")
            .arg(openCount)
            .arg(dueCount)
            .arg(readyCount)
            .arg(completedCount)
            .arg(m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'"))
            .arg(m_db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'"))
            .arg(m_db->countWhere("alerts_escalation_items", "status='Open' OR status='Due Today' OR status='Blocked'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("huddle_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* nameItem = new QTableWidgetItem(row.value("huddle_name"));
        auto* audienceItem = new QTableWidgetItem(row.value("audience_name"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* timeItem = new QTableWidgetItem(row.value("due_time"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        QString status = row.value("status");
        if (status == "Ready") statusItem->setBackground(QColor("#dff3e4"));
        else if (status == "Completed") statusItem->setBackground(QColor("#e9f2ff"));
        else statusItem->setBackground(QColor("#fff4cc"));

        QString tip = row.value("top_priorities");
        const QString notes = row.value("facilitator_notes");
        if (!notes.isEmpty()) {
            if (!tip.isEmpty()) tip += "\n\n";
            tip += "Notes: " + notes;
        }
        for (QTableWidgetItem* item : {dateItem, nameItem, audienceItem, ownerItem, timeItem, statusItem}) {
            item->setToolTip(tip);
        }

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, nameItem);
        m_table->setItem(r, 2, audienceItem);
        m_table->setItem(r, 3, ownerItem);
        m_table->setItem(r, 4, timeItem);
        m_table->setItem(r, 5, statusItem);
    }
}
