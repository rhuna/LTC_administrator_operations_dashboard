#include "ResidentsPage.h"
#include "../../data/DatabaseManager.h"

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
#include <QVBoxLayout>

ResidentsPage::ResidentsPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Residents", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Track current census and discharge residents directly from the desktop dashboard.", this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    root->addWidget(heading);
    root->addWidget(subheading);

    auto* actionCard = new QGroupBox("Resident actions", this);
    auto* actionLayout = new QHBoxLayout(actionCard);
    actionLayout->setContentsMargins(14, 14, 14, 14);
    actionLayout->setSpacing(10);

    m_dischargeButton = new QPushButton("Discharge Selected Resident", actionCard);
    m_refreshButton = new QPushButton("Refresh", actionCard);
    actionLayout->addWidget(m_dischargeButton);
    actionLayout->addWidget(m_refreshButton);
    actionLayout->addStretch();
    root->addWidget(actionCard);

    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(5);
    m_tableWidget->setHorizontalHeaderLabels(QStringList{"id", "resident_name", "room", "payer", "status"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setColumnHidden(0, true);
    root->addWidget(m_tableWidget, 1);

    connect(m_refreshButton, &QPushButton::clicked, this, &ResidentsPage::refreshTable);
    connect(m_dischargeButton, &QPushButton::clicked, this, &ResidentsPage::handleDischargeResident);

    refreshTable();
}

void ResidentsPage::refreshTable() {
    m_tableWidget->setRowCount(0);
    const auto rows = m_db->fetchTable("residents", QStringList{"id", "resident_name", "room", "payer", "status"});
    for (const auto& row : rows) {
        int r = m_tableWidget->rowCount();
        m_tableWidget->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"id", "resident_name", "room", "payer", "status"}) {
            m_tableWidget->setItem(r, c++, new QTableWidgetItem(row.value(key)));
        }
    }
}

void ResidentsPage::handleDischargeResident() {
    const int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Discharge resident", "Select a resident row first.");
        return;
    }

    const int residentId = m_tableWidget->item(row, 0)->text().toInt();
    const QString residentName = m_tableWidget->item(row, 1)->text();
    const QString currentStatus = m_tableWidget->item(row, 4)->text();

    if (currentStatus.compare("Discharged", Qt::CaseInsensitive) == 0) {
        QMessageBox::information(this, "Discharge resident", "That resident is already marked discharged.");
        return;
    }

    const auto result = QMessageBox::question(
        this,
        "Confirm discharge",
        QString("Mark %1 as discharged?").arg(residentName));

    if (result != QMessageBox::Yes) {
        return;
    }

    if (!m_db->dischargeResident(residentId, residentName)) {
        QMessageBox::warning(this, "Discharge resident", "Unable to update the resident record.");
        return;
    }

    refreshTable();
}
