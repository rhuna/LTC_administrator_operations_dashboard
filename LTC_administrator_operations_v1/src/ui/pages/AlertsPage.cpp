#include "AlertsPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

AlertsPage::AlertsPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Notifications / Due-Soon Alerts", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "See overdue and due-soon operational items in one place so follow-up work does not stay buried inside individual modules.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* summary = new QLabel(
        QString("%1 overdue item(s) · %2 due-soon item(s)")
            .arg(db->overdueAlertCount())
            .arg(db->dueSoonAlertCount()),
        this);
    summary->setStyleSheet("color:#486581; font-weight:600; padding:4px 0 8px 0;");
    root->addWidget(summary);

    auto* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(6);
    tableWidget->setHorizontalHeaderLabels({"Urgency", "Module", "Item", "Due / Date", "Owner", "Status"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    const auto rows = db->alertItems();
    tableWidget->setRowCount(rows.size());
    const QStringList cols{"urgency", "module", "item", "due_date", "owner", "status"};
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < cols.size(); ++c) {
            tableWidget->setItem(r, c, new QTableWidgetItem(rows[r].value(cols[c])));
        }
    }

    root->addWidget(tableWidget, 1);
}
