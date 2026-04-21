#include "SurveyReadinessPage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
SurveyReadinessPage::SurveyReadinessPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    auto* heading = new QLabel("Survey Readiness / Compliance", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);
    auto* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels(QStringList{"focus_area", "owner", "risk_level", "status"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto rows = db->fetchTable("survey_items", QStringList{"focus_area", "owner", "risk_level", "status"});
    for (const auto& row : rows) {
        int r = tableWidget->rowCount();
        tableWidget->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"focus_area", "owner", "risk_level", "status"}) tableWidget->setItem(r, c++, new QTableWidgetItem(row.value(key)));
    }

    root->addWidget(tableWidget);
}

    auto* complianceHeading = new QLabel("Compliance items", this);
    complianceHeading->setStyleSheet("font-size: 16px; font-weight: 700;");
    root->addWidget(complianceHeading);
    auto* complianceTable = new QTableWidget(this);
    complianceTable->setColumnCount(4);
    complianceTable->setHorizontalHeaderLabels(QStringList{"item_name", "due_date", "owner", "status"});
    complianceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto complianceRows = db->fetchTable("compliance_items", QStringList{"item_name", "due_date", "owner", "status"});
    for (const auto& row : complianceRows) {
        int r = complianceTable->rowCount();
        complianceTable->insertRow(r);
        int c = 0;
        for (const auto& key : QStringList{"item_name", "due_date", "owner", "status"}) complianceTable->setItem(r, c++, new QTableWidgetItem(row.value(key)));
    }
    root->addWidget(complianceTable);
