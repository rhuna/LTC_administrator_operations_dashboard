
#include "AlertsPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
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

    auto* summary = new QLabel(QString("%1 overdue item(s) · %2 due-soon item(s)").arg(db->overdueAlertCount()).arg(db->dueSoonAlertCount()), this);
    summary->setStyleSheet("color:#486581; font-weight:600; padding:4px 0 8px 0;");
    root->addWidget(summary);

    auto* board = new QTableWidget(this);
    const QStringList boardCols{"urgency", "module", "item", "due_date", "owner", "status"};
    board->setColumnCount(boardCols.size());
    board->setHorizontalHeaderLabels({"Urgency", "Module", "Item", "Due / Date", "Owner", "Status"});
    board->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    board->setEditTriggers(QAbstractItemView::NoEditTriggers);
    board->setSelectionBehavior(QAbstractItemView::SelectRows);
    root->addWidget(board, 1);

    auto* manualLabel = new QLabel("Direct alerts", this);
    manualLabel->setStyleSheet("font-size: 16px; font-weight: 700;");
    root->addWidget(manualLabel);

    auto* manual = new QTableWidget(this);
    const QStringList manualCols{"id","alert_date","module_name","item_name","owner","status"};
    manual->setColumnCount(manualCols.size());
    manual->setHorizontalHeaderLabels({"ID","Date","Module","Item","Owner","Status"});
    manual->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    manual->setEditTriggers(QAbstractItemView::NoEditTriggers);
    manual->setSelectionBehavior(QAbstractItemView::SelectRows);
    root->addWidget(manual);

    auto* form = new QHBoxLayout();
    auto* date = new QLineEdit(this); date->setPlaceholderText("YYYY-MM-DD");
    auto* module = new QLineEdit(this); module->setPlaceholderText("Module");
    auto* item = new QLineEdit(this); item->setPlaceholderText("Alert item");
    auto* owner = new QLineEdit(this); owner->setPlaceholderText("Owner");
    auto* addBtn = new QPushButton("Add Alert", this);
    auto* delBtn = new QPushButton("Delete Selected", this);
    for (auto* w : {date,module,item,owner}) form->addWidget(w);
    form->addWidget(addBtn);
    form->addWidget(delBtn);
    root->addLayout(form);

    auto refresh = [=]() {
        board->setRowCount(0);
        const auto rows = db->alertItems();
        for (int r = 0; r < rows.size(); ++r) {
            board->insertRow(r);
            for (int c = 0; c < boardCols.size(); ++c) board->setItem(r, c, new QTableWidgetItem(rows[r].value(boardCols[c])));
        }

        manual->setRowCount(0);
        const auto mrows = db->fetchTable("alerts_items", manualCols);
        for (int r = 0; r < mrows.size(); ++r) {
            manual->insertRow(r);
            for (int c = 0; c < manualCols.size(); ++c) manual->setItem(r, c, new QTableWidgetItem(mrows[r].value(manualCols[c])));
        }
    };

    QObject::connect(addBtn,&QPushButton::clicked,this,[=]() {
        QMap<QString,QString> v{{"alert_date",date->text()},{"module_name",module->text()},{"item_name",item->text()},{"owner",owner->text()},{"status","Open"}};
        if (db->addRecord("alerts_items", v)) { date->clear(); module->clear(); item->clear(); owner->clear(); refresh(); }
    });
    QObject::connect(delBtn,&QPushButton::clicked,this,[=]() {
        const int r = manual->currentRow(); if (r < 0) return;
        db->deleteRecordById("alerts_items", manual->item(r,0)->text().toInt());
        refresh();
    });

    refresh();
}
