#include "SurveyReadinessPage.h"
#include "../../data/DatabaseManager.h"

#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

SurveyReadinessPage::SurveyReadinessPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Survey Readiness / Compliance", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* surveyHeading = new QLabel("Survey readiness items", this);
    surveyHeading->setStyleSheet("font-size: 16px; font-weight: 700;");
    root->addWidget(surveyHeading);

    auto* surveyTable = new QTableWidget(this);
    const QStringList surveyCols{"focus_area", "owner", "risk_level", "status"};
    surveyTable->setColumnCount(surveyCols.size());
    surveyTable->setHorizontalHeaderLabels({"Focus Area", "Owner", "Risk", "Status"});
    surveyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    const auto surveyRows = db->fetchTable("survey_items", surveyCols);
    for (const auto& row : surveyRows) {
        const int r = surveyTable->rowCount();
        surveyTable->insertRow(r);
        for (int c = 0; c < surveyCols.size(); ++c) {
            surveyTable->setItem(r, c, new QTableWidgetItem(row.value(surveyCols[c])));
        }
    }
    root->addWidget(surveyTable);

    auto* complianceHeading = new QLabel("Compliance items", this);
    complianceHeading->setStyleSheet("font-size: 16px; font-weight: 700;");
    root->addWidget(complianceHeading);

    auto* complianceTable = new QTableWidget(this);
    const QStringList complianceCols{"item_name", "due_date", "owner", "status"};
    complianceTable->setColumnCount(complianceCols.size());
    complianceTable->setHorizontalHeaderLabels({"Item", "Due Date", "Owner", "Status"});
    complianceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    const auto complianceRows = db->fetchTable("compliance_items", complianceCols);
    for (const auto& row : complianceRows) {
        const int r = complianceTable->rowCount();
        complianceTable->insertRow(r);
        for (int c = 0; c < complianceCols.size(); ++c) {
            complianceTable->setItem(r, c, new QTableWidgetItem(row.value(complianceCols[c])));
        }
    }
    root->addWidget(complianceTable);
}
