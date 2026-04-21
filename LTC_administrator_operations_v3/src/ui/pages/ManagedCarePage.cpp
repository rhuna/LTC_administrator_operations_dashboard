#include "ManagedCarePage.h"
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

ManagedCarePage::ManagedCarePage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Managed Care / Billing", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Track authorization requests, expiry dates, clinical update deadlines, and denial follow-up "
        "for managed-care payers. Add new items, update status, and monitor the authorization queue.",
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
    auto* formCard = new QGroupBox("Add managed care item", this);
    auto* formLayout = new QFormLayout(formCard);
    formCard->setStyleSheet("QGroupBox { font-weight: 600; }");

    auto* residentEdit    = new QLineEdit(formCard);
    auto* payerEdit       = new QLineEdit(formCard);
    auto* itemEdit        = new QLineEdit(formCard);
    auto* authExpEdit     = new QLineEdit(formCard);
    auto* ownerEdit       = new QLineEdit(formCard);
    auto* statusCombo     = new QComboBox(formCard);

    residentEdit->setPlaceholderText("Resident name");
    payerEdit->setPlaceholderText("Payer / managed care plan");
    itemEdit->setPlaceholderText("Authorization item or follow-up description");
    authExpEdit->setPlaceholderText("Auth expiry date YYYY-MM-DD (if applicable)");
    ownerEdit->setPlaceholderText("Owner");
    statusCombo->addItems({"Open", "Pending Auth", "Auth Approved", "Auth Denied",
                           "Appeal Filed", "At Risk", "Watch", "Closed"});

    formLayout->addRow("Resident", residentEdit);
    formLayout->addRow("Payer", payerEdit);
    formLayout->addRow("Item / auth", itemEdit);
    formLayout->addRow("Auth expiry", authExpEdit);
    formLayout->addRow("Owner", ownerEdit);
    formLayout->addRow("Status", statusCombo);

    auto* addButton = new QPushButton("Add Item", formCard);
    formLayout->addRow(addButton);
    root->addWidget(formCard);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels(
        {"Resident", "Payer", "Item / Auth", "Auth Expiry", "Owner", "Status"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    // Action bar
    auto* actionBar = new QHBoxLayout();
    auto* approveButton = new QPushButton("Mark Auth Approved", this);
    auto* deleteButton  = new QPushButton("Delete Selected", this);
    deleteButton->setStyleSheet("background:#c0392b;");
    auto* refreshButton = new QPushButton("Refresh", this);
    actionBar->addWidget(approveButton);
    actionBar->addWidget(deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(refreshButton);
    root->addLayout(actionBar);

    refreshTable();

    connect(addButton, &QPushButton::clicked, this, [=]() {
        const QString resident = residentEdit->text().trimmed();
        const QString item     = itemEdit->text().trimmed();
        if (resident.isEmpty() || item.isEmpty()) {
            QMessageBox::warning(this, "Missing fields", "Resident name and item description are required.");
            return;
        }
        QMap<QString, QString> vals;
        vals["resident_name"] = resident;
        vals["payer"]         = payerEdit->text().trimmed();
        vals["item_name"]     = item;
        vals["auth_expiry"]   = authExpEdit->text().trimmed();
        vals["owner"]         = ownerEdit->text().trimmed();
        vals["status"]        = statusCombo->currentText();
        if (m_db->addRecord("managed_care_items", vals)) {
            residentEdit->clear(); payerEdit->clear(); itemEdit->clear();
            authExpEdit->clear(); ownerEdit->clear();
            statusCombo->setCurrentIndex(0);
            refreshTable();
        } else {
            QMessageBox::warning(this, "Error", "Could not save the managed care item.");
        }
    });

    connect(approveButton, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        m_db->updateRecordById("managed_care_items", id, {{"status", "Auth Approved"}});
        refreshTable();
    });

    connect(deleteButton, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        if (QMessageBox::question(this, "Confirm delete", "Delete this managed care item?") == QMessageBox::Yes) {
            m_db->deleteRecordById("managed_care_items", id);
            refreshTable();
        }
    });

    connect(refreshButton, &QPushButton::clicked, this, &ManagedCarePage::refreshTable);
}

void ManagedCarePage::refreshTable() {
    m_table->setRowCount(0);

    // Fetch with auth_expiry column — use legacy cols if column doesn't exist yet
    const QStringList cols{"id", "resident_name", "payer", "item_name", "auth_expiry", "owner", "status"};
    const QStringList legacyCols{"id", "resident_name", "payer", "item_name", "status"};
    auto rows = m_db->fetchTable("managed_care_items", cols);
    bool hasAuthExpiry = true;
    if (rows.isEmpty()) {
        rows = m_db->fetchTable("managed_care_items", legacyCols);
        hasAuthExpiry = false;
    }

    int openCount  = 0;
    int atRisk     = 0;
    int pendingAuth = 0;

    const QStringList displayCols{"resident_name", "payer", "item_name", "auth_expiry", "owner", "status"};
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);
        const QString status = row.value("status");
        if (status == "Open" || status == "Pending Auth") { ++openCount; if (status == "Pending Auth") ++pendingAuth; }
        if (status == "At Risk" || status == "Auth Denied") ++atRisk;

        for (int c = 0; c < displayCols.size(); ++c) {
            QString val = row.value(displayCols[c]);
            if (displayCols[c] == "auth_expiry" && !hasAuthExpiry) val = "";
            auto* item = new QTableWidgetItem(val);
            if (c == 0) item->setData(Qt::UserRole, row.value("id").toInt());
            if (displayCols[c] == "status") {
                if (status == "At Risk" || status == "Auth Denied")
                    item->setForeground(QColor("#b91c1c"));
                else if (status == "Auth Approved" || status == "Closed")
                    item->setForeground(QColor("#166534"));
                else if (status == "Watch" || status == "Pending Auth")
                    item->setForeground(QColor("#92400e"));
            }
            m_table->setItem(r, c, item);
        }
    }

    m_summaryLabel->setText(
        QString("%1 open / pending auth  ·  %2 pending auth  ·  %3 at risk / denied  ·  %4 total")
            .arg(openCount).arg(pendingAuth).arg(atRisk).arg(rows.size()));
}
