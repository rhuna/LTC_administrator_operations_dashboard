#include "QapiPage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
QapiPage::QapiPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    auto* heading = new QLabel("QAPI / PIP", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);
    auto* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels(QStringList{"project_name", "owner", "status", "next_step"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto rows = db->fetchTable("pips", QStringList{"project_name", "owner", "status", "next_step"});
    for (const auto& row : rows) {
        int r = tableWidget->rowCount();
        tableWidget->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"project_name", "owner", "status", "next_step"}) tableWidget->setItem(r, c++, new QTableWidgetItem(row.value(key)));
    }

    root->addWidget(tableWidget);
}
