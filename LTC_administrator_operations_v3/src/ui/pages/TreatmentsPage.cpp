
#include "TreatmentsPage.h"
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

TreatmentsPage::TreatmentsPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8,8,8,8);
    root->setSpacing(14);

    auto* heading = new QLabel("Treatments / Wounds", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subtitle = new QLabel("Track wound treatments in one place. Add new wound rows, resolve selected items, or delete items that should be removed.", this);
    subtitle->setWordWrap(true);
    root->addWidget(heading);
    root->addWidget(subtitle);

    auto* table = new QTableWidget(this);
    const QStringList cols{"id","review_date","resident_name","wound_name","location","status","notes"};
    table->setColumnCount(cols.size());
    table->setHorizontalHeaderLabels({"ID","Date","Resident","Wound","Location","Status","Notes"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    root->addWidget(table,1);

    auto* form = new QHBoxLayout();
    auto* date = new QLineEdit(this); date->setPlaceholderText("YYYY-MM-DD");
    auto* resident = new QLineEdit(this); resident->setPlaceholderText("Resident");
    auto* wound = new QLineEdit(this); wound->setPlaceholderText("Wound");
    auto* location = new QLineEdit(this); location->setPlaceholderText("Location");
    auto* notes = new QLineEdit(this); notes->setPlaceholderText("Notes");
    auto* addBtn = new QPushButton("Add Wound", this);
    auto* resolveBtn = new QPushButton("Resolve Selected", this);
    auto* deleteBtn = new QPushButton("Delete Selected", this);
    for (auto* w : {date,resident,wound,location,notes}) form->addWidget(w);
    form->addWidget(addBtn);
    form->addWidget(resolveBtn);
    form->addWidget(deleteBtn);
    root->addLayout(form);

    auto refresh = [=]() {
        table->setRowCount(0);
        const auto rows = db->fetchTable("wound_treatments", cols);
        for (const auto& row : rows) {
            int r = table->rowCount();
            table->insertRow(r);
            for (int c=0;c<cols.size();++c) table->setItem(r,c,new QTableWidgetItem(row.value(cols[c])));
        }
    };

    QObject::connect(addBtn,&QPushButton::clicked,this,[=]() {
        QMap<QString,QString> v{{"review_date",date->text()},{"resident_name",resident->text()},{"wound_name",wound->text()},{"location",location->text()},{"status","Open"},{"notes",notes->text()}};
        if (db->addRecord("wound_treatments", v)) { date->clear(); resident->clear(); wound->clear(); location->clear(); notes->clear(); refresh(); }
    });
    QObject::connect(resolveBtn,&QPushButton::clicked,this,[=]() {
        const int r=table->currentRow(); if (r<0) return;
        const int id=table->item(r,0)->text().toInt();
        db->updateRecordById("wound_treatments", id, {{"status","Resolved"}});
        refresh();
    });
    QObject::connect(deleteBtn,&QPushButton::clicked,this,[=]() {
        const int r=table->currentRow(); if (r<0) return;
        const int id=table->item(r,0)->text().toInt();
        db->deleteRecordById("wound_treatments", id);
        refresh();
    });

    refresh();
}
