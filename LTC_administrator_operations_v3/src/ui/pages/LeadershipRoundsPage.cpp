#include "LeadershipRoundsPage.h"
#include "../../data/DatabaseManager.h"

#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

LeadershipRoundsPage::LeadershipRoundsPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Leadership Rounds & Daily Ops Brief", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v66 adds an executive rounds workspace so the administrator, DON, admissions, and department leaders can capture round notes, assign follow-up, and keep a visible daily brief without removing the existing workflow pages.",
        this);
    subheading->setWordWrap(true);

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet("font-weight: 600; color: #334155;");

    root->addWidget(heading);
    root->addWidget(subheading);
    root->addWidget(m_summaryLabel);

    auto* strip = new QHBoxLayout();
    auto makeStat = [&](const QString& title, QLabel*& valueLabel) {
        auto* card = new QGroupBox(title, this);
        auto* layout = new QVBoxLayout(card);
        valueLabel = new QLabel("0", card);
        valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #102a43;");
        auto* hint = new QLabel("Live count from leadership rounds log", card);
        hint->setStyleSheet("color: #52606d;");
        hint->setWordWrap(true);
        layout->addWidget(valueLabel);
        layout->addWidget(hint);
        strip->addWidget(card);
    };
    makeStat("Open follow-up", m_openFollowupLabel);
    makeStat("High priority", m_highPriorityLabel);
    makeStat("Completed today", m_completedLabel);
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Add leadership round note", this);
    auto* formLayout = new QFormLayout(formCard);

    m_roundDateEdit = new QLineEdit(formCard);
    m_shiftEdit = new QLineEdit(formCard);
    m_areaEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_priorityEdit = new QLineEdit(formCard);
    m_statusEdit = new QLineEdit(formCard);
    m_followupDateEdit = new QLineEdit(formCard);
    m_noteEdit = new QTextEdit(formCard);

    m_roundDateEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_shiftEdit->setText("Morning Leadership Rounds");
    m_areaEdit->setPlaceholderText("Admissions / Staffing / Dining / Survey / Clinical / EVS");
    m_ownerEdit->setPlaceholderText("Administrator / DON / Department Head");
    m_priorityEdit->setText("High");
    m_statusEdit->setText("Open");
    m_followupDateEdit->setText(QDate::currentDate().addDays(1).toString("yyyy-MM-dd"));
    m_noteEdit->setPlaceholderText("Document what was found, what follow-up is needed, and what the leadership team should watch on the next round.");
    m_noteEdit->setMinimumHeight(90);

    formLayout->addRow("Round date:", m_roundDateEdit);
    formLayout->addRow("Round / shift:", m_shiftEdit);
    formLayout->addRow("Area:", m_areaEdit);
    formLayout->addRow("Owner:", m_ownerEdit);
    formLayout->addRow("Priority:", m_priorityEdit);
    formLayout->addRow("Status:", m_statusEdit);
    formLayout->addRow("Follow-up due:", m_followupDateEdit);
    formLayout->addRow("Round note:", m_noteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save round note", formCard);
    m_refreshButton = new QPushButton("Refresh brief", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_refreshButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);

    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Leadership rounds log", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels(QStringList{
        "round_date", "shift_name", "area_name", "owner_name", "priority", "status", "followup_date", "round_note"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &LeadershipRoundsPage::addRoundNote);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &LeadershipRoundsPage::refreshView);

    refreshView();
}

void LeadershipRoundsPage::addRoundNote() {
    if (!m_db) return;
    if (m_areaEdit->text().trimmed().isEmpty() || m_noteEdit->toPlainText().trimmed().isEmpty()) return;

    m_db->addRecord("leadership_rounds", {
        {"round_date", m_roundDateEdit->text()},
        {"shift_name", m_shiftEdit->text()},
        {"area_name", m_areaEdit->text()},
        {"owner_name", m_ownerEdit->text()},
        {"priority", m_priorityEdit->text()},
        {"status", m_statusEdit->text()},
        {"followup_date", m_followupDateEdit->text()},
        {"round_note", m_noteEdit->toPlainText()}
    });

    m_noteEdit->clear();
    m_areaEdit->clear();
    m_priorityEdit->setText("High");
    m_statusEdit->setText("Open");
    refreshView();
}

void LeadershipRoundsPage::refreshView() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "leadership_rounds",
        {"round_date", "shift_name", "area_name", "owner_name", "priority", "status", "followup_date", "round_note", "id"});

    const int openCount = m_db->countWhere("leadership_rounds", "status='Open' OR status='In Progress' OR status='Watch'");
    const int highCount = m_db->countWhere("leadership_rounds", "priority='High' OR priority='Urgent'");
    const int completeToday = m_db->countWhere(
        "leadership_rounds",
        QString("round_date='%1' AND (status='Complete' OR status='Closed')").arg(QDate::currentDate().toString("yyyy-MM-dd")));

    m_openFollowupLabel->setText(QString::number(openCount));
    m_highPriorityLabel->setText(QString::number(highCount));
    m_completedLabel->setText(QString::number(completeToday));

    m_summaryLabel->setText(
        QString("Live brief: %1 current residents · %2 open staffing assignments · %3 admit-ready referrals · %4 overdue alerts · %5 active survey items · %6 leadership round follow-up items")
            .arg(m_db->countWhere("residents", "status='Current'"))
            .arg(m_db->countWhere("staffing_assignments", "status='Open'"))
            .arg(m_db->countWhere("admissions", "status='Ready'"))
            .arg(m_db->overdueAlertCount())
            .arg(m_db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'"))
            .arg(openCount));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);
        m_table->setItem(r, 0, new QTableWidgetItem(row.value("round_date")));
        m_table->setItem(r, 1, new QTableWidgetItem(row.value("shift_name")));
        m_table->setItem(r, 2, new QTableWidgetItem(row.value("area_name")));
        m_table->setItem(r, 3, new QTableWidgetItem(row.value("owner_name")));
        m_table->setItem(r, 4, new QTableWidgetItem(row.value("priority")));
        m_table->setItem(r, 5, new QTableWidgetItem(row.value("status")));
        m_table->setItem(r, 6, new QTableWidgetItem(row.value("followup_date")));
        m_table->setItem(r, 7, new QTableWidgetItem(row.value("round_note")));
    }
}
