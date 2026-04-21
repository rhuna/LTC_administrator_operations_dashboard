#include "BudgetPage.h"
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

BudgetPage::BudgetPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Budget / Labor", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Track budget items by department with variance flags and status. "
        "Add new items, update status, and keep leadership informed on labor and operational spend.",
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
    auto* formCard = new QGroupBox("Add budget item", this);
    auto* formLayout = new QFormLayout(formCard);
    formCard->setStyleSheet("QGroupBox { font-weight: 600; }");

    auto* itemEdit       = new QLineEdit(formCard);
    auto* deptEdit       = new QLineEdit(formCard);
    auto* varianceEdit   = new QLineEdit(formCard);
    auto* statusCombo    = new QComboBox(formCard);

    itemEdit->setPlaceholderText("Budget item name");
    deptEdit->setPlaceholderText("Department");
    varianceEdit->setPlaceholderText("Variance (e.g. +12%, -5%, On Target)");
    statusCombo->addItems({"On Target", "Watch", "At Risk", "Over Budget", "Under Budget", "Closed"});

    formLayout->addRow("Item name", itemEdit);
    formLayout->addRow("Department", deptEdit);
    formLayout->addRow("Variance", varianceEdit);
    formLayout->addRow("Status", statusCombo);

    auto* addButton = new QPushButton("Add Item", formCard);
    formLayout->addRow(addButton);
    root->addWidget(formCard);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Item Name", "Department", "Variance", "Status"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    // Action bar
    auto* actionBar = new QHBoxLayout();
    auto* closeButton   = new QPushButton("Close Selected", this);
    auto* deleteButton  = new QPushButton("Delete Selected", this);
    deleteButton->setStyleSheet("background:#c0392b;");
    auto* refreshButton = new QPushButton("Refresh", this);
    actionBar->addWidget(closeButton);
    actionBar->addWidget(deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(refreshButton);
    root->addLayout(actionBar);

    refreshTable();

    connect(addButton, &QPushButton::clicked, this, [=]() {
        const QString item = itemEdit->text().trimmed();
        if (item.isEmpty()) {
            QMessageBox::warning(this, "Missing fields", "Item name is required.");
            return;
        }
        QMap<QString, QString> vals;
        vals["item_name"]   = item;
        vals["department"]  = deptEdit->text().trimmed();
        vals["variance"]    = varianceEdit->text().trimmed();
        vals["status"]      = statusCombo->currentText();
        if (m_db->addRecord("budget_items", vals)) {
            itemEdit->clear(); deptEdit->clear(); varianceEdit->clear();
            statusCombo->setCurrentIndex(0);
            refreshTable();
        } else {
            QMessageBox::warning(this, "Error", "Could not save the budget item.");
        }
    });

    connect(closeButton, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        m_db->updateRecordById("budget_items", id, {{"status", "Closed"}});
        refreshTable();
    });

    connect(deleteButton, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        if (QMessageBox::question(this, "Confirm delete", "Delete this budget item?") == QMessageBox::Yes) {
            m_db->deleteRecordById("budget_items", id);
            refreshTable();
        }
    });

    connect(refreshButton, &QPushButton::clicked, this, &BudgetPage::refreshTable);
}

void BudgetPage::refreshTable() {
    m_table->setRowCount(0);
    const QStringList cols{"id", "item_name", "department", "variance", "status"};
    const auto rows = m_db->fetchTable("budget_items", cols);

    int atRisk = 0, watch = 0, onTarget = 0;
    const QStringList displayCols{"item_name", "department", "variance", "status"};

    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);
        const QString status = row.value("status");
        if (status == "At Risk" || status == "Over Budget") ++atRisk;
        else if (status == "Watch") ++watch;
        else if (status == "On Target" || status == "Under Budget") ++onTarget;

        for (int c = 0; c < displayCols.size(); ++c) {
            auto* item = new QTableWidgetItem(row.value(displayCols[c]));
            if (c == 0) item->setData(Qt::UserRole, row.value("id").toInt());
            if (displayCols[c] == "status") {
                if (status == "At Risk" || status == "Over Budget")
                    item->setForeground(QColor("#b91c1c"));
                else if (status == "On Target" || status == "Under Budget")
                    item->setForeground(QColor("#166534"));
                else if (status == "Watch")
                    item->setForeground(QColor("#92400e"));
            }
            m_table->setItem(r, c, item);
        }
    }

    m_summaryLabel->setText(
        QString("%1 on target  ·  %2 watch  ·  %3 at risk / over budget  ·  %4 total items")
            .arg(onTarget).arg(watch).arg(atRisk).arg(rows.size()));
}
