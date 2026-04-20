#include "IncidentsPage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
IncidentsPage::IncidentsPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    auto* heading = new QLabel("Incidents", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);
    auto* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(5);
    tableWidget->setHorizontalHeaderLabels(QStringList{"incident_date", "resident_name", "incident_type", "severity", "status"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto rows = db->fetchTable("incidents", QStringList{"incident_date", "resident_name", "incident_type", "severity", "status"});
    for (const auto& row : rows) {
        int r = tableWidget->rowCount();
        tableWidget->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"incident_date", "resident_name", "incident_type", "severity", "status"}) tableWidget->setItem(r, c++, new QTableWidgetItem(row.value(key)));
    }

    root->addWidget(tableWidget);
}
