#include "QualityMeasuresPage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
QualityMeasuresPage::QualityMeasuresPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    auto* heading = new QLabel("Quality Measures", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);
    auto* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels(QStringList{"measure_name", "current_value", "target_value", "status"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto rows = db->fetchTable("quality_measures", QStringList{"measure_name", "current_value", "target_value", "status"});
    for (const auto& row : rows) {
        int r = tableWidget->rowCount();
        tableWidget->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"measure_name", "current_value", "target_value", "status"}) tableWidget->setItem(r, c++, new QTableWidgetItem(row.value(key)));
    }

    root->addWidget(tableWidget);
}
