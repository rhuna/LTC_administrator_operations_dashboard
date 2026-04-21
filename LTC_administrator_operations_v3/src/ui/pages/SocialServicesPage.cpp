#include "SocialServicesPage.h"
#include "../../data/DatabaseManager.h"

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
    const QStringList socialCols{"review_date", "resident_name", "focus_area", "item_name", "owner", "status", "notes"};
    socialTable->setColumnCount(socialCols.size());
    socialTable->setHorizontalHeaderLabels({"Date", "Resident", "Focus Area", "Item", "Owner", "Status", "Notes"});
    socialTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    socialLayout->addWidget(socialTable);

    auto* socialForm = new QHBoxLayout();
    auto* dateEdit = new QLineEdit(this);
    dateEdit->setPlaceholderText("YYYY-MM-DD");
    auto* residentName = new QLineEdit(this);
    residentName->setPlaceholderText("Resident");
    auto* focusArea = new QLineEdit(this);
    focusArea->setPlaceholderText("Discharge / Family / Psychosocial");
    auto* itemName = new QLineEdit(this);
    itemName->setPlaceholderText("Item / follow-up");
    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Owner");
    auto* notes = new QLineEdit(this);
    notes->setPlaceholderText("Notes");
    auto* socialButton = new QPushButton("Add Social Services Item", this);
    socialForm->addWidget(dateEdit);
    socialForm->addWidget(residentName);
    socialForm->addWidget(focusArea);
    socialForm->addWidget(itemName);
    socialForm->addWidget(owner);
    socialForm->addWidget(notes);
    socialForm->addWidget(socialButton);
    socialLayout->addLayout(socialForm);
    root->addWidget(socialGroup);

    auto* conferenceGroup = new QGroupBox("Care conferences", this);
    auto* conferenceLayout = new QVBoxLayout(conferenceGroup);
    auto* conferenceTable = new QTableWidget(this);
    const QStringList conferenceCols{"conference_date", "resident_name", "contact_name", "conference_type", "owner_name", "status", "notes"};
    conferenceTable->setColumnCount(conferenceCols.size());
    conferenceTable->setHorizontalHeaderLabels({"Date", "Resident", "Contact", "Conference Type", "Owner", "Status", "Notes"});
    conferenceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    conferenceLayout->addWidget(conferenceTable);

    auto* conferenceForm = new QHBoxLayout();
    auto* conferenceDate = new QLineEdit(this);
    conferenceDate->setPlaceholderText("YYYY-MM-DD");
    auto* conferenceResident = new QLineEdit(this);
    conferenceResident->setPlaceholderText("Resident");
    auto* conferenceContact = new QLineEdit(this);
    conferenceContact->setPlaceholderText("Family / responsible party");
    auto* conferenceType = new QLineEdit(this);
    conferenceType->setPlaceholderText("Conference type");
    auto* conferenceOwner = new QLineEdit(this);
    conferenceOwner->setPlaceholderText("Owner");
    auto* conferenceNotes = new QLineEdit(this);
    conferenceNotes->setPlaceholderText("Notes");
    auto* conferenceButton = new QPushButton("Add Care Conference", this);
    conferenceForm->addWidget(conferenceDate);
    conferenceForm->addWidget(conferenceResident);
    conferenceForm->addWidget(conferenceContact);
    conferenceForm->addWidget(conferenceType);
    conferenceForm->addWidget(conferenceOwner);
    conferenceForm->addWidget(conferenceNotes);
    conferenceForm->addWidget(conferenceButton);
    conferenceLayout->addLayout(conferenceForm);
    root->addWidget(conferenceGroup);

    auto* grievanceGroup = new QGroupBox("Grievances / service recovery", this);
    auto* grievanceLayout = new QVBoxLayout(grievanceGroup);
    auto* grievanceTable = new QTableWidget(this);
    const QStringList grievanceCols{"report_date", "category", "resident_or_family", "owner", "priority", "status", "summary"};
    grievanceTable->setColumnCount(grievanceCols.size());
    grievanceTable->setHorizontalHeaderLabels({"Date", "Category", "Resident / Family", "Owner", "Priority", "Status", "Summary"});
    grievanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    grievanceLayout->addWidget(grievanceTable);

    auto* grievanceForm = new QHBoxLayout();
    auto* grievanceDate = new QLineEdit(this);
    grievanceDate->setPlaceholderText("YYYY-MM-DD");
    auto* grievanceCategory = new QLineEdit(this);
    grievanceCategory->setPlaceholderText("Category");
    auto* grievanceParty = new QLineEdit(this);
    grievanceParty->setPlaceholderText("Resident / family");
    auto* grievanceOwner = new QLineEdit(this);
    grievanceOwner->setPlaceholderText("Owner");
    auto* grievanceSummary = new QLineEdit(this);
    grievanceSummary->setPlaceholderText("Summary");
    auto* grievanceButton = new QPushButton("Add Grievance", this);
    grievanceForm->addWidget(grievanceDate);
    grievanceForm->addWidget(grievanceCategory);
    grievanceForm->addWidget(grievanceParty);
    grievanceForm->addWidget(grievanceOwner);
    grievanceForm->addWidget(grievanceSummary);
    grievanceForm->addWidget(grievanceButton);
    grievanceLayout->addLayout(grievanceForm);
    root->addWidget(grievanceGroup, 1);

    auto refreshTables = [=]() {
        socialTable->setRowCount(0);
        for (const auto& row : db->fetchTable("social_services_items", socialCols)) {
            const int r = socialTable->rowCount();
            socialTable->insertRow(r);
            for (int c = 0; c < socialCols.size(); ++c) socialTable->setItem(r, c, new QTableWidgetItem(row.value(socialCols[c])));
        }

        conferenceTable->setRowCount(0);
        for (const auto& row : db->fetchTable("care_conference_items", conferenceCols)) {
            const int r = conferenceTable->rowCount();
            conferenceTable->insertRow(r);
            for (int c = 0; c < conferenceCols.size(); ++c) conferenceTable->setItem(r, c, new QTableWidgetItem(row.value(conferenceCols[c])));
        }

        grievanceTable->setRowCount(0);
        for (const auto& row : db->fetchTable("grievances", grievanceCols)) {
            const int r = grievanceTable->rowCount();
            grievanceTable->insertRow(r);
            for (int c = 0; c < grievanceCols.size(); ++c) grievanceTable->setItem(r, c, new QTableWidgetItem(row.value(grievanceCols[c])));
        }
    };

    QObject::connect(socialButton, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values;
        values["review_date"] = dateEdit->text().trimmed();
        values["resident_name"] = residentName->text().trimmed();
        values["focus_area"] = focusArea->text().trimmed();
        values["item_name"] = itemName->text().trimmed();
        values["owner"] = owner->text().trimmed();
        values["status"] = "Open";
        values["notes"] = notes->text().trimmed();
        if (db->addRecord("social_services_items", values)) {
            dateEdit->clear(); residentName->clear(); focusArea->clear(); itemName->clear(); owner->clear(); notes->clear();
            refreshTables();
        }
    });

    QObject::connect(conferenceButton, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values;
        values["conference_date"] = conferenceDate->text().trimmed();
        values["resident_name"] = conferenceResident->text().trimmed();
        values["contact_name"] = conferenceContact->text().trimmed();
        values["conference_type"] = conferenceType->text().trimmed();
        values["owner_name"] = conferenceOwner->text().trimmed();
        values["status"] = "Open";
        values["notes"] = conferenceNotes->text().trimmed();
        if (db->addRecord("care_conference_items", values)) {
            conferenceDate->clear(); conferenceResident->clear(); conferenceContact->clear(); conferenceType->clear(); conferenceOwner->clear(); conferenceNotes->clear();
            refreshTables();
        }
    });

    QObject::connect(grievanceButton, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values;
        values["report_date"] = grievanceDate->text().trimmed();
        values["category"] = grievanceCategory->text().trimmed();
        values["resident_or_family"] = grievanceParty->text().trimmed();
        values["owner"] = grievanceOwner->text().trimmed();
        values["priority"] = "Medium";
        values["status"] = "Open";
        values["summary"] = grievanceSummary->text().trimmed();
        if (db->addRecord("grievances", values)) {
            grievanceDate->clear(); grievanceCategory->clear(); grievanceParty->clear(); grievanceOwner->clear(); grievanceSummary->clear();
            refreshTables();
        }
    });

    refreshTables();
}
