#include "RevenueCyclePage.h"
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

RevenueCyclePage::RevenueCyclePage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Revenue Cycle", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Track payer mix, billing queue items, denial follow-up, and A/R aging in one financial operations workspace.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    root->addWidget(heading);
    root->addWidget(subheading);

    // Summary strip
    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet(
        "background:#eef4f8; border:1px solid #d9e2ec; border-radius:10px;"
        "padding:8px 14px; color:#334e68; font-weight:600;");
    root->addWidget(m_summaryLabel);

    // Add form
    auto* formCard = new QGroupBox("Add revenue cycle item", this);
    auto* formLayout = new QFormLayout(formCard);
    formCard->setStyleSheet("QGroupBox { font-weight: 600; }");

    auto* residentEdit   = new QLineEdit(formCard);
    auto* payerEdit      = new QLineEdit(formCard);
    auto* itemEdit       = new QLineEdit(formCard);
    auto* arAgingEdit    = new QLineEdit(formCard);
    auto* ownerEdit      = new QLineEdit(formCard);
    auto* statusCombo    = new QComboBox(formCard);

    residentEdit->setPlaceholderText("Resident name");
    payerEdit->setPlaceholderText("Payer / insurance");
    itemEdit->setPlaceholderText("Billing item or denial description");
    arAgingEdit->setPlaceholderText("A/R aging bucket (e.g. 0-30, 31-60, 61-90, 90+)");
    ownerEdit->setPlaceholderText("Owner");
    statusCombo->addItems({"Open", "In Progress", "Pending Auth", "Denial", "Appeal Filed",
                           "At Risk", "Watch", "Resolved", "Closed"});

    formLayout->addRow("Resident", residentEdit);
    formLayout->addRow("Payer", payerEdit);
    formLayout->addRow("Item / denial", itemEdit);
    formLayout->addRow("A/R aging", arAgingEdit);
    formLayout->addRow("Owner", ownerEdit);
    formLayout->addRow("Status", statusCombo);

    auto* addButton = new QPushButton("Add Item", formCard);
    formLayout->addRow(addButton);
    root->addWidget(formCard);

    // Table
    const QStringList cols{"resident_name", "payer", "item_name", "ar_aging", "owner", "status"};
    m_table = new QTableWidget(this);
    m_table->setColumnCount(cols.size());
    m_table->setHorizontalHeaderLabels({"Resident", "Payer", "Item / Denial", "A/R Aging", "Owner", "Status"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    // Action bar
    auto* actionBar = new QHBoxLayout();
    auto* resolveButton = new QPushButton("Mark Resolved", this);
    auto* deleteButton  = new QPushButton("Delete Selected", this);
    deleteButton->setStyleSheet("background:#c0392b;");
    auto* refreshButton = new QPushButton("Refresh", this);
    actionBar->addWidget(resolveButton);
    actionBar->addWidget(deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(refreshButton);
    root->addLayout(actionBar);

    refreshTable();

    // Connections
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
        vals["ar_aging"]      = arAgingEdit->text().trimmed();
        vals["owner"]         = ownerEdit->text().trimmed();
        vals["status"]        = statusCombo->currentText();
        if (m_db->addRecord("revenue_cycle_items", vals)) {
            residentEdit->clear(); payerEdit->clear(); itemEdit->clear();
            arAgingEdit->clear(); ownerEdit->clear();
            statusCombo->setCurrentIndex(0);
            refreshTable();
        } else {
            QMessageBox::warning(this, "Error", "Could not save the revenue cycle item.");
        }
    });

    connect(resolveButton, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        m_db->updateRecordById("revenue_cycle_items", id, {{"status", "Resolved"}});
        refreshTable();
    });

    connect(deleteButton, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        if (QMessageBox::question(this, "Confirm delete", "Delete this revenue cycle item?") == QMessageBox::Yes) {
            m_db->deleteRecordById("revenue_cycle_items", id);
            refreshTable();
        }
    });

    connect(refreshButton, &QPushButton::clicked, this, &RevenueCyclePage::refreshTable);
}

void RevenueCyclePage::refreshTable() {
    m_table->setRowCount(0);
    const QStringList cols{"id", "resident_name", "payer", "item_name", "ar_aging", "owner", "status"};
    const auto rows = m_db->fetchTable("revenue_cycle_items", cols);

    int openCount    = 0;
    int denialCount  = 0;
    int atRiskCount  = 0;

    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        const QString status = row.value("status");
        if (status == "Open" || status == "In Progress" || status == "Pending Auth") ++openCount;
        if (status == "Denial" || status == "Appeal Filed") ++denialCount;
        if (status == "At Risk") ++atRiskCount;

        const QStringList displayCols{"resident_name", "payer", "item_name", "ar_aging", "owner", "status"};
        for (int c = 0; c < displayCols.size(); ++c) {
            auto* item = new QTableWidgetItem(row.value(displayCols[c]));
            if (c == 0) item->setData(Qt::UserRole, row.value("id").toInt());
            if (displayCols[c] == "status") {
                if (status == "Denial" || status == "At Risk")
                    item->setForeground(QColor("#b91c1c"));
                else if (status == "Resolved" || status == "Closed")
                    item->setForeground(QColor("#166534"));
                else if (status == "Watch" || status == "Pending Auth")
                    item->setForeground(QColor("#92400e"));
            }
            m_table->setItem(r, c, item);
        }
    }

    m_summaryLabel->setText(
        QString("%1 open / in-progress  ·  %2 denial / appeal  ·  %3 at risk  ·  %4 total items")
            .arg(openCount).arg(denialCount).arg(atRiskCount).arg(rows.size()));
}
