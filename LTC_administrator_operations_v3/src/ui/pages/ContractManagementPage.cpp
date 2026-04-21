#include "ContractManagementPage.h"
#include "../../data/DatabaseManager.h"

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

ContractManagementPage::ContractManagementPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Contract Management", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Track vendor contracts, renewal dates, rate schedules, and escalation clauses so nothing slips past its deadline.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    root->addWidget(heading);
    root->addWidget(subheading);

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet(
        "background:#eef4f8; border:1px solid #d9e2ec; border-radius:10px;"
        "padding:8px 14px; color:#334e68; font-weight:600;");
    root->addWidget(m_summaryLabel);

    // Add form
    auto* formCard = new QGroupBox("Add contract", this);
    auto* formLayout = new QFormLayout(formCard);
    formCard->setStyleSheet("QGroupBox { font-weight: 600; }");

    auto* vendorEdit      = new QLineEdit(formCard);
    auto* categoryEdit    = new QLineEdit(formCard);
    auto* renewalEdit     = new QLineEdit(formCard);
    auto* rateEdit        = new QLineEdit(formCard);
    auto* ownerEdit       = new QLineEdit(formCard);
    auto* statusCombo     = new QComboBox(formCard);
    auto* notesEdit       = new QLineEdit(formCard);

    vendorEdit->setPlaceholderText("Vendor or contractor name");
    categoryEdit->setPlaceholderText("Category (e.g. Agency Staffing, Pharmacy, Dietary, Therapy)");
    renewalEdit->setPlaceholderText("Renewal date YYYY-MM-DD");
    rateEdit->setPlaceholderText("Rate schedule summary");
    ownerEdit->setPlaceholderText("Contract owner");
    notesEdit->setPlaceholderText("Escalation clause, notice period, or key notes");
    statusCombo->addItems({"Active", "Up for Renewal", "Pending Signature", "Watch",
                           "Expired", "Terminated", "Archived"});

    formLayout->addRow("Vendor", vendorEdit);
    formLayout->addRow("Category", categoryEdit);
    formLayout->addRow("Renewal date", renewalEdit);
    formLayout->addRow("Rate schedule", rateEdit);
    formLayout->addRow("Owner", ownerEdit);
    formLayout->addRow("Status", statusCombo);
    formLayout->addRow("Notes", notesEdit);

    auto* addButton = new QPushButton("Add Contract", formCard);
    formLayout->addRow(addButton);
    root->addWidget(formCard);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(
        {"Vendor", "Category", "Renewal Date", "Rate Schedule", "Owner", "Status", "Notes"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    // Action bar
    auto* actionBar = new QHBoxLayout();
    auto* archiveButton = new QPushButton("Archive Selected", this);
    auto* deleteButton  = new QPushButton("Delete Selected", this);
    deleteButton->setStyleSheet("background:#c0392b;");
    auto* refreshButton = new QPushButton("Refresh", this);
    actionBar->addWidget(archiveButton);
    actionBar->addWidget(deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(refreshButton);
    root->addLayout(actionBar);

    refreshTable();

    connect(addButton, &QPushButton::clicked, this, [=]() {
        const QString vendor = vendorEdit->text().trimmed();
        if (vendor.isEmpty()) {
            QMessageBox::warning(this, "Missing fields", "Vendor name is required.");
            return;
        }
        QMap<QString, QString> vals;
        vals["vendor_name"]     = vendor;
        vals["category"]        = categoryEdit->text().trimmed();
        vals["renewal_date"]    = renewalEdit->text().trimmed();
        vals["rate_schedule"]   = rateEdit->text().trimmed();
        vals["owner"]           = ownerEdit->text().trimmed();
        vals["status"]          = statusCombo->currentText();
        vals["notes"]           = notesEdit->text().trimmed();
        if (m_db->addRecord("contracts", vals)) {
            vendorEdit->clear(); categoryEdit->clear(); renewalEdit->clear();
            rateEdit->clear(); ownerEdit->clear(); notesEdit->clear();
            statusCombo->setCurrentIndex(0);
            refreshTable();
        } else {
            QMessageBox::warning(this, "Error", "Could not save the contract.");
        }
    });

    connect(archiveButton, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        m_db->archiveRecordById("contracts", id);
        refreshTable();
    });

    connect(deleteButton, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        if (QMessageBox::question(this, "Confirm delete", "Delete this contract record?") == QMessageBox::Yes) {
            m_db->deleteRecordById("contracts", id);
            refreshTable();
        }
    });

    connect(refreshButton, &QPushButton::clicked, this, &ContractManagementPage::refreshTable);
}

void ContractManagementPage::refreshTable() {
    m_table->setRowCount(0);
    const QStringList cols{"id", "vendor_name", "category", "renewal_date",
                           "rate_schedule", "owner", "status", "notes"};
    const auto rows = m_db->fetchTable("contracts", cols);

    int activeCount  = 0;
    int renewalCount = 0;
    int watchCount   = 0;

    const QStringList displayCols{"vendor_name", "category", "renewal_date",
                                  "rate_schedule", "owner", "status", "notes"};
    for (const auto& row : rows) {
        if (row.value("status") == "Archived") continue;
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        const QString status = row.value("status");
        if (status == "Active") ++activeCount;
        if (status == "Up for Renewal" || status == "Pending Signature") ++renewalCount;
        if (status == "Watch" || status == "Expired") ++watchCount;

        for (int c = 0; c < displayCols.size(); ++c) {
            auto* item = new QTableWidgetItem(row.value(displayCols[c]));
            if (c == 0) item->setData(Qt::UserRole, row.value("id").toInt());
            if (displayCols[c] == "status") {
                if (status == "Expired" || status == "Terminated")
                    item->setForeground(QColor("#b91c1c"));
                else if (status == "Active")
                    item->setForeground(QColor("#166534"));
                else if (status == "Up for Renewal" || status == "Watch")
                    item->setForeground(QColor("#92400e"));
            }
            m_table->setItem(r, c, item);
        }
    }

    m_summaryLabel->setText(
        QString("%1 active  ·  %2 up for renewal / pending  ·  %3 watch / expired  ·  %4 total")
            .arg(activeCount).arg(renewalCount).arg(watchCount).arg(activeCount + renewalCount + watchCount));
}
