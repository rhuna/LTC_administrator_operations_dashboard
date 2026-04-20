#include "PreparednessPage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
PreparednessPage::PreparednessPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    auto* heading = new QLabel("Emergency Preparedness", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);
    auto* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels(QStringList{"item_name", "due_date", "owner", "status"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto rows = db->fetchTable("preparedness_items", QStringList{"item_name", "due_date", "owner", "status"});
    for (const auto& row : rows) {
        int r = tableWidget->rowCount();
        tableWidget->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"item_name", "due_date", "owner", "status"}) tableWidget->setItem(r, c++, new QTableWidgetItem(row.value(key)));
    }

    root->addWidget(tableWidget);
}
