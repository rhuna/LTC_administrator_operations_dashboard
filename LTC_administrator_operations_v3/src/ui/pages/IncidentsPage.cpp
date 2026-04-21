
#include "IncidentsPage.h"
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
void fillRows(QTableWidget* table, const QList<QMap<QString, QString>>& rows, const QStringList& cols) {
    table->setRowCount(0);
    for (const auto& row : rows) {
        int r = table->rowCount();
        table->insertRow(r);
        for (int c = 0; c < cols.size(); ++c) table->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
    }
}
}

IncidentsPage::IncidentsPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    auto* heading = new QLabel("DON / Incidents / Interventions / Reportables", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subtitle = new QLabel("The DON workspace keeps incidents, clinical interventions, and diagnosis reportables together without duplicating social-services or medical-record tasks.", this);
    subtitle->setWordWrap(true);
    root->addWidget(heading);
    root->addWidget(subtitle);

    auto* incGroup = new QGroupBox("Incidents", this);
    auto* incLayout = new QVBoxLayout(incGroup);
    auto* incidents = new QTableWidget(this);
    const QStringList icols{"id","incident_date","resident_name","incident_type","severity","status"};
    incidents->setColumnCount(icols.size());
    incidents->setHorizontalHeaderLabels({"ID","Date","Resident","Incident","Severity","Status"});
    incidents->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    incidents->setSelectionBehavior(QAbstractItemView::SelectRows);
    incLayout->addWidget(incidents);
    auto* iform = new QHBoxLayout();
    auto *idate=new QLineEdit(this),*ires=new QLineEdit(this),*itype=new QLineEdit(this),*isev=new QLineEdit(this);
    idate->setPlaceholderText("YYYY-MM-DD"); ires->setPlaceholderText("Resident"); itype->setPlaceholderText("Incident"); isev->setPlaceholderText("Severity");
    auto *iadd=new QPushButton("Add",this), *idel=new QPushButton("Delete Selected",this);
    for (auto* w : {idate,ires,itype,isev}) iform->addWidget(w); iform->addWidget(iadd); iform->addWidget(idel);
    incLayout->addLayout(iform);
    root->addWidget(incGroup);

    auto* intGroup = new QGroupBox("Interventions", this);
    auto* intLayout = new QVBoxLayout(intGroup);
    auto* interventions = new QTableWidget(this);
    const QStringList tcols{"id","review_date","resident_name","intervention_name","owner","status","notes"};
    interventions->setColumnCount(tcols.size());
    interventions->setHorizontalHeaderLabels({"ID","Date","Resident","Intervention","Owner","Status","Notes"});
    interventions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    interventions->setSelectionBehavior(QAbstractItemView::SelectRows);
    intLayout->addWidget(interventions);
    auto* tform = new QHBoxLayout();
    auto *tdate=new QLineEdit(this),*tres=new QLineEdit(this),*tname=new QLineEdit(this),*towner=new QLineEdit(this),*tnotes=new QLineEdit(this);
    tdate->setPlaceholderText("YYYY-MM-DD"); tres->setPlaceholderText("Resident"); tname->setPlaceholderText("Intervention"); towner->setPlaceholderText("Owner"); tnotes->setPlaceholderText("Notes");
    auto *tadd=new QPushButton("Add",this), *tdel=new QPushButton("Delete Selected",this);
    for (auto* w : {tdate,tres,tname,towner,tnotes}) tform->addWidget(w); tform->addWidget(tadd); tform->addWidget(tdel);
    intLayout->addLayout(tform);
    root->addWidget(intGroup);

    auto* repGroup = new QGroupBox("Diagnosis reportables", this);
    auto* repLayout = new QVBoxLayout(repGroup);
    auto* reportables = new QTableWidget(this);
    const QStringList rcols{"id","review_date","resident_name","diagnosis_name","reportable_flag","owner","status","notes"};
    reportables->setColumnCount(rcols.size());
    reportables->setHorizontalHeaderLabels({"ID","Date","Resident","Diagnosis","Reportable","Owner","Status","Notes"});
    reportables->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    reportables->setSelectionBehavior(QAbstractItemView::SelectRows);
    repLayout->addWidget(reportables);
    auto* rform = new QHBoxLayout();
    auto *rdate=new QLineEdit(this),*rres=new QLineEdit(this),*rdiag=new QLineEdit(this),*rflag=new QLineEdit(this),*rowner=new QLineEdit(this),*rnotes=new QLineEdit(this);
    rdate->setPlaceholderText("YYYY-MM-DD"); rres->setPlaceholderText("Resident"); rdiag->setPlaceholderText("Diagnosis"); rflag->setPlaceholderText("Yes / No"); rowner->setPlaceholderText("Owner"); rnotes->setPlaceholderText("Notes");
    auto *radd=new QPushButton("Add",this), *rdel=new QPushButton("Delete Selected",this);
    for (auto* w : {rdate,rres,rdiag,rflag,rowner,rnotes}) rform->addWidget(w); rform->addWidget(radd); rform->addWidget(rdel);
    repLayout->addLayout(rform);
    root->addWidget(repGroup);

    auto refresh = [=]() {
        fillRows(incidents, db->fetchTable("incidents", icols), icols);
        fillRows(interventions, db->fetchTable("interventions", tcols), tcols);
        fillRows(reportables, db->fetchTable("diagnosis_reportables", rcols), rcols);
    };

    QObject::connect(iadd,&QPushButton::clicked,this,[=](){ if(db->addRecord("incidents",{{"incident_date",idate->text()},{"resident_name",ires->text()},{"incident_type",itype->text()},{"severity",isev->text()},{"status","Open"}})) refresh(); });
    QObject::connect(idel,&QPushButton::clicked,this,[=](){ int r=incidents->currentRow(); if(r>=0){ db->deleteRecordById("incidents", incidents->item(r,0)->text().toInt()); refresh(); }});

    QObject::connect(tadd,&QPushButton::clicked,this,[=](){ if(db->addRecord("interventions",{{"review_date",tdate->text()},{"resident_name",tres->text()},{"intervention_name",tname->text()},{"owner",towner->text()},{"status","Open"},{"notes",tnotes->text()}})) refresh(); });
    QObject::connect(tdel,&QPushButton::clicked,this,[=](){ int r=interventions->currentRow(); if(r>=0){ db->deleteRecordById("interventions", interventions->item(r,0)->text().toInt()); refresh(); }});

    QObject::connect(radd,&QPushButton::clicked,this,[=](){ if(db->addRecord("diagnosis_reportables",{{"review_date",rdate->text()},{"resident_name",rres->text()},{"diagnosis_name",rdiag->text()},{"reportable_flag",rflag->text()},{"owner",rowner->text()},{"status","Open"},{"notes",rnotes->text()}})) refresh(); });
    QObject::connect(rdel,&QPushButton::clicked,this,[=](){ int r=reportables->currentRow(); if(r>=0){ db->deleteRecordById("diagnosis_reportables", reportables->item(r,0)->text().toInt()); refresh(); }});

    refresh();
}
