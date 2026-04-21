
#include "SocialServicesPage.h"
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

SocialServicesPage::SocialServicesPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Social Services / Care Conferences / Grievances", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Use one combined social-services workspace for discharge planning, family communication, care conferences, and grievances so those resident/family coordination items live together.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* socialGroup = new QGroupBox("Social services worklist", this);
    auto* socialLayout = new QVBoxLayout(socialGroup);
    auto* socialTable = new QTableWidget(this);
    const QStringList socialCols{"id","review_date", "resident_name", "focus_area", "item_name", "owner", "status", "notes"};
    socialTable->setColumnCount(socialCols.size());
    socialTable->setHorizontalHeaderLabels({"ID","Date", "Resident", "Focus Area", "Item", "Owner", "Status", "Notes"});
    socialTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    socialTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    socialLayout->addWidget(socialTable);

    auto* socialForm = new QHBoxLayout();
    auto* dateEdit = new QLineEdit(this); dateEdit->setPlaceholderText("YYYY-MM-DD");
    auto* residentName = new QLineEdit(this); residentName->setPlaceholderText("Resident");
    auto* focusArea = new QLineEdit(this); focusArea->setPlaceholderText("Discharge / Family / Psychosocial");
    auto* itemName = new QLineEdit(this); itemName->setPlaceholderText("Item / follow-up");
    auto* owner = new QLineEdit(this); owner->setPlaceholderText("Owner");
    auto* notes = new QLineEdit(this); notes->setPlaceholderText("Notes");
    auto* socialButton = new QPushButton("Add Social Services Item", this);
    auto* socialDelete = new QPushButton("Delete Selected", this);
    for (auto* w : {dateEdit,residentName,focusArea,itemName,owner,notes}) socialForm->addWidget(w);
    socialForm->addWidget(socialButton);
    socialForm->addWidget(socialDelete);
    socialLayout->addLayout(socialForm);
    root->addWidget(socialGroup);

    auto* conferenceGroup = new QGroupBox("Care conferences", this);
    auto* conferenceLayout = new QVBoxLayout(conferenceGroup);
    auto* conferenceTable = new QTableWidget(this);
    const QStringList conferenceCols{"id","conference_date", "resident_name", "contact_name", "conference_type", "owner_name", "status", "notes"};
    conferenceTable->setColumnCount(conferenceCols.size());
    conferenceTable->setHorizontalHeaderLabels({"ID","Date", "Resident", "Contact", "Conference", "Owner", "Status", "Notes"});
    conferenceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    conferenceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    conferenceLayout->addWidget(conferenceTable);

    auto* conferenceForm = new QHBoxLayout();
    auto* conferenceDate = new QLineEdit(this); conferenceDate->setPlaceholderText("YYYY-MM-DD");
    auto* conferenceResident = new QLineEdit(this); conferenceResident->setPlaceholderText("Resident");
    auto* conferenceContact = new QLineEdit(this); conferenceContact->setPlaceholderText("Family / POA / contact");
    auto* conferenceType = new QLineEdit(this); conferenceType->setPlaceholderText("Quarterly / Discharge / Behavior");
    auto* conferenceOwner = new QLineEdit(this); conferenceOwner->setPlaceholderText("Owner");
    auto* conferenceNotes = new QLineEdit(this); conferenceNotes->setPlaceholderText("Notes");
    auto* conferenceButton = new QPushButton("Add Care Conference", this);
    auto* conferenceDelete = new QPushButton("Delete Selected", this);
    for (auto* w : {conferenceDate,conferenceResident,conferenceContact,conferenceType,conferenceOwner,conferenceNotes}) conferenceForm->addWidget(w);
    conferenceForm->addWidget(conferenceButton);
    conferenceForm->addWidget(conferenceDelete);
    conferenceLayout->addLayout(conferenceForm);
    root->addWidget(conferenceGroup);

    auto* grievanceGroup = new QGroupBox("Grievances", this);
    auto* grievanceLayout = new QVBoxLayout(grievanceGroup);
    auto* grievanceTable = new QTableWidget(this);
    const QStringList grievanceCols{"id","report_date", "category", "resident_or_family", "owner", "priority", "status", "summary"};
    grievanceTable->setColumnCount(grievanceCols.size());
    grievanceTable->setHorizontalHeaderLabels({"ID","Date", "Category", "Resident / Family", "Owner", "Priority", "Status", "Summary"});
    grievanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    grievanceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    grievanceLayout->addWidget(grievanceTable);

    auto* grievanceForm = new QHBoxLayout();
    auto* grievanceDate = new QLineEdit(this); grievanceDate->setPlaceholderText("YYYY-MM-DD");
    auto* grievanceCategory = new QLineEdit(this); grievanceCategory->setPlaceholderText("Category");
    auto* grievanceParty = new QLineEdit(this); grievanceParty->setPlaceholderText("Resident / family");
    auto* grievanceOwner = new QLineEdit(this); grievanceOwner->setPlaceholderText("Owner");
    auto* grievanceSummary = new QLineEdit(this); grievanceSummary->setPlaceholderText("Summary");
    auto* grievanceButton = new QPushButton("Add Grievance", this);
    auto* grievanceDelete = new QPushButton("Delete Selected", this);
    for (auto* w : {grievanceDate,grievanceCategory,grievanceParty,grievanceOwner,grievanceSummary}) grievanceForm->addWidget(w);
    grievanceForm->addWidget(grievanceButton);
    grievanceForm->addWidget(grievanceDelete);
    grievanceLayout->addLayout(grievanceForm);
    root->addWidget(grievanceGroup);

    auto refreshTables = [=]() {
        socialTable->setRowCount(0);
        for (const auto& row : db->fetchTable("social_services_items", socialCols)) {
            const int r = socialTable->rowCount(); socialTable->insertRow(r);
            for (int c = 0; c < socialCols.size(); ++c) socialTable->setItem(r, c, new QTableWidgetItem(row.value(socialCols[c])));
        }
        conferenceTable->setRowCount(0);
        for (const auto& row : db->fetchTable("care_conference_items", conferenceCols)) {
            const int r = conferenceTable->rowCount(); conferenceTable->insertRow(r);
            for (int c = 0; c < conferenceCols.size(); ++c) conferenceTable->setItem(r, c, new QTableWidgetItem(row.value(conferenceCols[c])));
        }
        grievanceTable->setRowCount(0);
        for (const auto& row : db->fetchTable("grievances", grievanceCols)) {
            const int r = grievanceTable->rowCount(); grievanceTable->insertRow(r);
            for (int c = 0; c < grievanceCols.size(); ++c) grievanceTable->setItem(r, c, new QTableWidgetItem(row.value(grievanceCols[c])));
        }
    };

    QObject::connect(socialButton, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{{"review_date",dateEdit->text().trimmed()},{"resident_name",residentName->text().trimmed()},{"focus_area",focusArea->text().trimmed()},{"item_name",itemName->text().trimmed()},{"owner",owner->text().trimmed()},{"status","Open"},{"notes",notes->text().trimmed()}};
        if (db->addRecord("social_services_items", values)) { dateEdit->clear(); residentName->clear(); focusArea->clear(); itemName->clear(); owner->clear(); notes->clear(); refreshTables(); }
    });
    QObject::connect(socialDelete,&QPushButton::clicked,this,[=]() {
        int r=socialTable->currentRow(); if(r<0) return; db->deleteRecordById("social_services_items", socialTable->item(r,0)->text().toInt()); refreshTables();
    });

    QObject::connect(conferenceButton, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{{"conference_date",conferenceDate->text().trimmed()},{"resident_name",conferenceResident->text().trimmed()},{"contact_name",conferenceContact->text().trimmed()},{"conference_type",conferenceType->text().trimmed()},{"owner_name",conferenceOwner->text().trimmed()},{"status","Open"},{"notes",conferenceNotes->text().trimmed()}};
        if (db->addRecord("care_conference_items", values)) { conferenceDate->clear(); conferenceResident->clear(); conferenceContact->clear(); conferenceType->clear(); conferenceOwner->clear(); conferenceNotes->clear(); refreshTables(); }
    });
    QObject::connect(conferenceDelete,&QPushButton::clicked,this,[=]() {
        int r=conferenceTable->currentRow(); if(r<0) return; db->deleteRecordById("care_conference_items", conferenceTable->item(r,0)->text().toInt()); refreshTables();
    });

    QObject::connect(grievanceButton, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{{"report_date",grievanceDate->text().trimmed()},{"category",grievanceCategory->text().trimmed()},{"resident_or_family",grievanceParty->text().trimmed()},{"owner",grievanceOwner->text().trimmed()},{"priority","Medium"},{"status","Open"},{"summary",grievanceSummary->text().trimmed()}};
        if (db->addRecord("grievances", values)) { grievanceDate->clear(); grievanceCategory->clear(); grievanceParty->clear(); grievanceOwner->clear(); grievanceSummary->clear(); refreshTables(); }
    });
    QObject::connect(grievanceDelete,&QPushButton::clicked,this,[=]() {
        int r=grievanceTable->currentRow(); if(r<0) return; db->deleteRecordById("grievances", grievanceTable->item(r,0)->text().toInt()); refreshTables();
    });

    refreshTables();
}
