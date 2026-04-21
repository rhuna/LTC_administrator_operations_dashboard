#include "OutbreakCommandPage.h"
#include "../../data/DatabaseManager.h"
#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

OutbreakCommandPage::OutbreakCommandPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Outbreak Command View", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Track outbreak-response work such as clusters, unit watch items, exposure follow-up, isolation readiness, and leadership response tasks in one focused command view.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* summary = new QLabel(
        QString("%1 active outbreak item(s) · %2 high-risk item(s)")
            .arg(db->countWhere("outbreak_items", "status!='Closed' AND status!='Complete'"))
            .arg(db->countWhere("outbreak_items", "priority='High' AND status!='Closed' AND status!='Complete'")),
        this);
    summary->setStyleSheet("color:#486581; font-weight:600; padding:4px 0 8px 0;");
    root->addWidget(summary);

    auto* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderLabels({"Cluster / Issue", "Location", "Cases", "Owner", "Priority", "Review Date", "Status"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    const auto rows = db->fetchTable("outbreak_items", {"issue_name", "location_name", "case_count", "owner", "priority", "review_date", "status"});
    tableWidget->setRowCount(rows.size());
    const QStringList cols{"issue_name", "location_name", "case_count", "owner", "priority", "review_date", "status"};
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < cols.size(); ++c) {
            tableWidget->setItem(r, c, new QTableWidgetItem(rows[r].value(cols[c])));
        }
    }

    root->addWidget(tableWidget, 1);
}
