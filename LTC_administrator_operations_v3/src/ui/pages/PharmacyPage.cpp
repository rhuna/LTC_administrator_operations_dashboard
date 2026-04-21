
#include "PharmacyPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
void fillTable(QTableWidget* table, const QList<QMap<QString, QString>>& rows, const QStringList& cols) {
    table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = table->rowCount();
        table->insertRow(r);
        for (int c = 0; c < cols.size(); ++c) table->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
    }
}
}

PharmacyPage::PharmacyPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Medical Records", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subtitle = new QLabel("Use one Medical Records page for pharmacy / med orders / DNR follow-up, infection-control records, vaccinations, isolations, outbreak review, and medical-record document intake.", this);
    subtitle->setWordWrap(true);
    root->addWidget(heading);
    root->addWidget(subtitle);

    auto* pharmacyGroup = new QGroupBox("Pharmacy / med orders / DNR", this);
    auto* pharmacyLayout = new QVBoxLayout(pharmacyGroup);
    auto* pharmacyTable = new QTableWidget(this);
    const QStringList pharmacyCols{"id","review_date","resident_name","item_name","owner","priority","status","notes"};
    pharmacyTable->setColumnCount(pharmacyCols.size());
    pharmacyTable->setHorizontalHeaderLabels({"ID","Date","Resident","Item","Owner","Priority","Status","Notes"});
    pharmacyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pharmacyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pharmacyLayout->addWidget(pharmacyTable);

    auto* pform = new QHBoxLayout();
    auto *pdate=new QLineEdit(this), *pres=new QLineEdit(this), *pitem=new QLineEdit(this), *powner=new QLineEdit(this), *pprio=new QLineEdit(this), *pnotes=new QLineEdit(this);
    pdate->setPlaceholderText("YYYY-MM-DD"); pres->setPlaceholderText("Resident"); pitem->setPlaceholderText("Med order / DNR / pharmacy item");
    powner->setPlaceholderText("Owner"); pprio->setPlaceholderText("Priority"); pnotes->setPlaceholderText("Notes");
    auto* padd=new QPushButton("Add",this); auto* pdel=new QPushButton("Delete Selected",this);
    for (auto* w : {pdate,pres,pitem,powner,pprio,pnotes}) pform->addWidget(w); pform->addWidget(padd); pform->addWidget(pdel);
    pharmacyLayout->addLayout(pform);
    root->addWidget(pharmacyGroup);

    auto* infectionGroup = new QGroupBox("Infection-control records", this);
    auto* infectionLayout = new QVBoxLayout(infectionGroup);
    auto* infectionTable = new QTableWidget(this);
    const QStringList infectionCols{"id","item_name","owner","status","notes"};
    infectionTable->setColumnCount(infectionCols.size());
    infectionTable->setHorizontalHeaderLabels({"ID","Item","Owner","Status","Notes"});
    infectionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    infectionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    infectionLayout->addWidget(infectionTable);
    auto* iform = new QHBoxLayout();
    auto *iitem=new QLineEdit(this), *iowner=new QLineEdit(this), *inotes=new QLineEdit(this);
    iitem->setPlaceholderText("Infection-control item"); iowner->setPlaceholderText("Owner"); inotes->setPlaceholderText("Notes");
    auto* iadd=new QPushButton("Add",this); auto* idel=new QPushButton("Delete Selected",this);
    for (auto* w : {iitem,iowner,inotes}) iform->addWidget(w); iform->addWidget(iadd); iform->addWidget(idel);
    infectionLayout->addLayout(iform);
    root->addWidget(infectionGroup);

    auto* vaccGroup = new QGroupBox("Vaccinations", this);
    auto* vaccLayout = new QVBoxLayout(vaccGroup);
    auto* vaccTable = new QTableWidget(this);
    const QStringList vaccCols{"id","review_date","resident_name","vaccine_name","status","notes"};
    vaccTable->setColumnCount(vaccCols.size());
    vaccTable->setHorizontalHeaderLabels({"ID","Date","Resident","Vaccine","Status","Notes"});
    vaccTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    vaccTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    vaccLayout->addWidget(vaccTable);
    auto* vform = new QHBoxLayout();
    auto *vdate=new QLineEdit(this), *vres=new QLineEdit(this), *vname=new QLineEdit(this), *vnotes=new QLineEdit(this);
    vdate->setPlaceholderText("YYYY-MM-DD"); vres->setPlaceholderText("Resident"); vname->setPlaceholderText("Vaccine"); vnotes->setPlaceholderText("Notes");
    auto* vadd=new QPushButton("Add",this); auto* vdel=new QPushButton("Delete Selected",this);
    for (auto* w : {vdate,vres,vname,vnotes}) vform->addWidget(w); vform->addWidget(vadd); vform->addWidget(vdel);
    vaccLayout->addLayout(vform);
    root->addWidget(vaccGroup);

    auto* isoGroup = new QGroupBox("Isolations", this);
    auto* isoLayout = new QVBoxLayout(isoGroup);
    auto* isoTable = new QTableWidget(this);
    const QStringList isoCols{"id","review_date","resident_name","isolation_type","status","notes"};
    isoTable->setColumnCount(isoCols.size());
    isoTable->setHorizontalHeaderLabels({"ID","Date","Resident","Isolation","Status","Notes"});
    isoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    isoTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    isoLayout->addWidget(isoTable);
    auto* isoform = new QHBoxLayout();
    auto *idate=new QLineEdit(this), *ires=new QLineEdit(this), *itype=new QLineEdit(this), *inote2=new QLineEdit(this);
    idate->setPlaceholderText("YYYY-MM-DD"); ires->setPlaceholderText("Resident"); itype->setPlaceholderText("Isolation type"); inote2->setPlaceholderText("Notes");
    auto* isoadd=new QPushButton("Add",this); auto* isodel=new QPushButton("Delete Selected",this);
    for (auto* w : {idate,ires,itype,inote2}) isoform->addWidget(w); isoform->addWidget(isoadd); isoform->addWidget(isodel);
    isoLayout->addLayout(isoform);
    root->addWidget(isoGroup);

    auto* outbreakGroup = new QGroupBox("Outbreak review", this);
    auto* outbreakLayout = new QVBoxLayout(outbreakGroup);
    auto* outbreakTable = new QTableWidget(this);
    const QStringList outbreakCols{"id","event_date","unit_name","event_name","status","notes"};
    outbreakTable->setColumnCount(outbreakCols.size());
    outbreakTable->setHorizontalHeaderLabels({"ID","Date","Unit","Event","Status","Notes"});
    outbreakTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    outbreakTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    outbreakLayout->addWidget(outbreakTable);
    auto* oform = new QHBoxLayout();
    auto *odate=new QLineEdit(this), *ounit=new QLineEdit(this), *oevent=new QLineEdit(this), *onotes=new QLineEdit(this);
    odate->setPlaceholderText("YYYY-MM-DD"); ounit->setPlaceholderText("Unit"); oevent->setPlaceholderText("Outbreak review item"); onotes->setPlaceholderText("Notes");
    auto* oadd=new QPushButton("Add",this); auto* odel=new QPushButton("Delete Selected",this);
    for (auto* w : {odate,ounit,oevent,onotes}) oform->addWidget(w); oform->addWidget(oadd); oform->addWidget(odel);
    outbreakLayout->addLayout(oform);
    root->addWidget(outbreakGroup);

    auto refreshTables = [=]() {
        fillTable(pharmacyTable, db->fetchTable("pharmacy_items", pharmacyCols), pharmacyCols);
        fillTable(infectionTable, db->fetchTable("infection_control_items", infectionCols), infectionCols);
        fillTable(vaccTable, db->fetchTable("vaccination_items", vaccCols), vaccCols);
        fillTable(isoTable, db->fetchTable("isolation_items", isoCols), isoCols);
        fillTable(outbreakTable, db->fetchTable("outbreak_items", outbreakCols), outbreakCols);
    };

    QObject::connect(padd,&QPushButton::clicked,this,[=](){
        if (db->addRecord("pharmacy_items", {{"review_date",pdate->text()},{"resident_name",pres->text()},{"item_name",pitem->text()},{"owner",powner->text()},{"priority",pprio->text().isEmpty()?"Medium":pprio->text()},{"status","Open"},{"notes",pnotes->text()}})) refreshTables();
    });
    QObject::connect(pdel,&QPushButton::clicked,this,[=](){ int r=pharmacyTable->currentRow(); if(r>=0){ db->deleteRecordById("pharmacy_items", pharmacyTable->item(r,0)->text().toInt()); refreshTables(); }});

    QObject::connect(iadd,&QPushButton::clicked,this,[=](){
        if (db->addRecord("infection_control_items", {{"item_name",iitem->text()},{"owner",iowner->text()},{"status","Open"},{"notes",inotes->text()}})) refreshTables();
    });
    QObject::connect(idel,&QPushButton::clicked,this,[=](){ int r=infectionTable->currentRow(); if(r>=0){ db->deleteRecordById("infection_control_items", infectionTable->item(r,0)->text().toInt()); refreshTables(); }});

    QObject::connect(vadd,&QPushButton::clicked,this,[=](){
        if (db->addRecord("vaccination_items", {{"review_date",vdate->text()},{"resident_name",vres->text()},{"vaccine_name",vname->text()},{"status","Open"},{"notes",vnotes->text()}})) refreshTables();
    });
    QObject::connect(vdel,&QPushButton::clicked,this,[=](){ int r=vaccTable->currentRow(); if(r>=0){ db->deleteRecordById("vaccination_items", vaccTable->item(r,0)->text().toInt()); refreshTables(); }});

    QObject::connect(isoadd,&QPushButton::clicked,this,[=](){
        if (db->addRecord("isolation_items", {{"review_date",idate->text()},{"resident_name",ires->text()},{"isolation_type",itype->text()},{"status","Open"},{"notes",inote2->text()}})) refreshTables();
    });
    QObject::connect(isodel,&QPushButton::clicked,this,[=](){ int r=isoTable->currentRow(); if(r>=0){ db->deleteRecordById("isolation_items", isoTable->item(r,0)->text().toInt()); refreshTables(); }});

    QObject::connect(oadd,&QPushButton::clicked,this,[=](){
        if (db->addRecord("outbreak_items", {{"event_date",odate->text()},{"unit_name",ounit->text()},{"event_name",oevent->text()},{"status","Open"},{"notes",onotes->text()}})) refreshTables();
    });
    QObject::connect(odel,&QPushButton::clicked,this,[=](){ int r=outbreakTable->currentRow(); if(r>=0){ db->deleteRecordById("outbreak_items", outbreakTable->item(r,0)->text().toInt()); refreshTables(); }});

    refreshTables();
}
