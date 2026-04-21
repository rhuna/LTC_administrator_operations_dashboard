#include "SurveyCommandCenterPage.h"
#include "../../data/DatabaseManager.h"
#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

SurveyCommandCenterPage::SurveyCommandCenterPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Survey Command Center", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Track active survey-preparation work, evidence collection, mock-survey findings, and plan-of-correction follow-up in one focused command view.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* summary = new QLabel(
        QString("%1 active survey command item(s) · %2 high-risk item(s)")
            .arg(db->countWhere("survey_command_items", "status!='Closed' AND status!='Complete'"))
            .arg(db->countWhere("survey_command_items", "priority='High' AND status!='Closed' AND status!='Complete'")),
        this);
    summary->setStyleSheet("color:#486581; font-weight:600; padding:4px 0 8px 0;");
    root->addWidget(summary);

    auto* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderLabels({"Area", "Finding / Item", "Evidence", "Owner", "Priority", "Due Date", "Status"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    const auto rows = db->fetchTable("survey_command_items", {"focus_area", "item_name", "evidence_needed", "owner", "priority", "due_date", "status"});
    tableWidget->setRowCount(rows.size());
    const QStringList cols{"focus_area", "item_name", "evidence_needed", "owner", "priority", "due_date", "status"};
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < cols.size(); ++c) {
            tableWidget->setItem(r, c, new QTableWidgetItem(rows[r].value(cols[c])));
        }
    }

    root->addWidget(tableWidget, 1);
}
