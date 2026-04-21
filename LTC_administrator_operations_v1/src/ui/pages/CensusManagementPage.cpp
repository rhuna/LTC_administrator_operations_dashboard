#include "CensusManagementPage.h"
#include "../../data/DatabaseManager.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

CensusManagementPage::CensusManagementPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Census Management / Stay Tracking", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Track bed holds, leave of absence, payer changes, room moves, and readmission-related census events in one simplified workspace.", this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    root->addWidget(heading);
    root->addWidget(subheading);

    auto* formCard = new QGroupBox("Add census event", this);
    auto* formLayout = new QFormLayout(formCard);
    formLayout->setContentsMargins(14, 14, 14, 14);
    formLayout->setSpacing(10);

    m_eventDateEdit = new QLineEdit(formCard);
    m_eventDateEdit->setPlaceholderText("YYYY-MM-DD");
    m_residentNameEdit = new QLineEdit(formCard);
    m_residentNameEdit->setPlaceholderText("Resident name");
    m_eventTypeEdit = new QLineEdit(formCard);
    m_eventTypeEdit->setPlaceholderText("Bed Hold / LOA / Room Move / Payer Change / Readmit");
    m_roomEdit = new QLineEdit(formCard);
    m_roomEdit->setPlaceholderText("Room");
    m_payerEdit = new QLineEdit(formCard);
    m_payerEdit->setPlaceholderText("Payer");
    m_statusEdit = new QLineEdit(formCard);
    m_statusEdit->setPlaceholderText("Open / Planned / Closed");
    m_notesEdit = new QLineEdit(formCard);
    m_notesEdit->setPlaceholderText("Notes");

    formLayout->addRow("Event date", m_eventDateEdit);
    formLayout->addRow("Resident", m_residentNameEdit);
    formLayout->addRow("Event type", m_eventTypeEdit);
    formLayout->addRow("Room", m_roomEdit);
    formLayout->addRow("Payer", m_payerEdit);
    formLayout->addRow("Status", m_statusEdit);
    formLayout->addRow("Notes", m_notesEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Add Census Event", formCard);
    m_refreshButton = new QPushButton("Refresh", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_refreshButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);

    root->addWidget(formCard);

    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(7);
    m_tableWidget->setHorizontalHeaderLabels({"Date", "Resident", "Event Type", "Room", "Payer", "Status", "Notes"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    root->addWidget(m_tableWidget, 1);

    connect(m_addButton, &QPushButton::clicked, this, &CensusManagementPage::handleAddEvent);
    connect(m_refreshButton, &QPushButton::clicked, this, &CensusManagementPage::refreshTable);

    refreshTable();
}

void CensusManagementPage::refreshTable() {
    m_tableWidget->setRowCount(0);
    const QStringList cols{"event_date", "resident_name", "event_type", "room", "payer", "status", "notes"};
    const auto rows = m_db->fetchTable("census_events", cols);
    for (const auto& row : rows) {
        const int r = m_tableWidget->rowCount();
        m_tableWidget->insertRow(r);
        for (int c = 0; c < cols.size(); ++c) {
            m_tableWidget->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
        }
    }
}

void CensusManagementPage::handleAddEvent() {
    m_db->addRecord("census_events", {
        {"event_date", m_eventDateEdit->text()},
        {"resident_name", m_residentNameEdit->text()},
        {"event_type", m_eventTypeEdit->text()},
        {"room", m_roomEdit->text()},
        {"payer", m_payerEdit->text()},
        {"status", m_statusEdit->text().isEmpty() ? QString("Open") : m_statusEdit->text()},
        {"notes", m_notesEdit->text()}
    });
    m_eventDateEdit->clear();
    m_residentNameEdit->clear();
    m_eventTypeEdit->clear();
    m_roomEdit->clear();
    m_payerEdit->clear();
    m_statusEdit->clear();
    m_notesEdit->clear();
    refreshTable();
}
