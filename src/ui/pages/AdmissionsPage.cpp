#include "AdmissionsPage.h"
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

AdmissionsPage::AdmissionsPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Admissions", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Create new referrals and admit selected residents into the current census without leaving the dashboard.", this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    root->addWidget(heading);
    root->addWidget(subheading);

    auto* formCard = new QGroupBox("Add referral / planned admission", this);
    auto* formLayout = new QFormLayout(formCard);
    formCard->setStyleSheet("QGroupBox { font-weight: 600; }");

    m_nameEdit = new QLineEdit(formCard);
    m_sourceEdit = new QLineEdit(formCard);
    m_dateEdit = new QLineEdit(formCard);
    m_statusEdit = new QLineEdit(formCard);
    m_roomEdit = new QLineEdit(formCard);
    m_payerEdit = new QLineEdit(formCard);

    m_nameEdit->setPlaceholderText("Resident name");
    m_sourceEdit->setPlaceholderText("Referral source");
    m_dateEdit->setPlaceholderText("YYYY-MM-DD");
    m_statusEdit->setPlaceholderText("Pending / Accepted");
    m_statusEdit->setText("Pending");
    m_roomEdit->setPlaceholderText("Room for admit action");
    m_payerEdit->setPlaceholderText("Payer for admit action");

    formLayout->addRow("Resident:", m_nameEdit);
    formLayout->addRow("Source:", m_sourceEdit);
    formLayout->addRow("Planned date:", m_dateEdit);
    formLayout->addRow("Status:", m_statusEdit);
    formLayout->addRow("Default room:", m_roomEdit);
    formLayout->addRow("Default payer:", m_payerEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Add Referral", formCard);
    m_admitButton = new QPushButton("Admit Selected", formCard);
    m_refreshButton = new QPushButton("Refresh", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_admitButton);
    buttonRow->addWidget(m_refreshButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);

    root->addWidget(formCard);

    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(5);
    m_tableWidget->setHorizontalHeaderLabels(QStringList{"id", "resident_name", "referral_source", "planned_date", "status"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setColumnHidden(0, true);
    root->addWidget(m_tableWidget, 1);

    connect(m_addButton, &QPushButton::clicked, this, &AdmissionsPage::handleAddReferral);
    connect(m_admitButton, &QPushButton::clicked, this, &AdmissionsPage::handleAdmitSelected);
    connect(m_refreshButton, &QPushButton::clicked, this, &AdmissionsPage::refreshTable);

    refreshTable();
}

void AdmissionsPage::refreshTable() {
    m_tableWidget->setRowCount(0);
    const auto rows = m_db->fetchTable("admissions", QStringList{"id", "resident_name", "referral_source", "planned_date", "status"});
    for (const auto& row : rows) {
        int r = m_tableWidget->rowCount();
        m_tableWidget->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"id", "resident_name", "referral_source", "planned_date", "status"}) {
            m_tableWidget->setItem(r, c++, new QTableWidgetItem(row.value(key)));
        }
    }
}

void AdmissionsPage::handleAddReferral() {
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::information(this, "Add referral", "Enter a resident name first.");
        return;
    }

    const bool ok = m_db->addRecord(
        "admissions",
        {
            {"resident_name", m_nameEdit->text().trimmed()},
            {"referral_source", m_sourceEdit->text().trimmed()},
            {"planned_date", m_dateEdit->text().trimmed()},
            {"status", m_statusEdit->text().trimmed().isEmpty() ? QString("Pending") : m_statusEdit->text().trimmed()}
        });

    if (!ok) {
        QMessageBox::warning(this, "Add referral", "Unable to save the new referral.");
        return;
    }

    refreshTable();
}

void AdmissionsPage::handleAdmitSelected() {
    const int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Admit resident", "Select an admission row first.");
        return;
    }

    const int admissionId = m_tableWidget->item(row, 0)->text().toInt();
    const QString residentName = m_tableWidget->item(row, 1)->text();
    const QString status = m_tableWidget->item(row, 4)->text();

    if (status.compare("Admitted", Qt::CaseInsensitive) == 0) {
        QMessageBox::information(this, "Admit resident", "That admission is already marked admitted.");
        return;
    }

    const QString room = m_roomEdit->text().trimmed().isEmpty() ? QString("TBD") : m_roomEdit->text().trimmed();
    const QString payer = m_payerEdit->text().trimmed().isEmpty() ? QString("Pending") : m_payerEdit->text().trimmed();

    if (!m_db->admitResident(residentName, room, payer, admissionId)) {
        QMessageBox::warning(this, "Admit resident", "Unable to admit the selected resident.");
        return;
    }

    refreshTable();
}
