
#include "PharmacyPage.h"
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

PharmacyPage::PharmacyPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Medical Records / Pharmacy / Infection Control / Outbreak", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel("Use one Medical Records page for pharmacy follow-up, infection-control record work, outbreak command visibility, and document intake items that belong with the clinical record.", this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* pharmacyGroup = new QGroupBox("Pharmacy and medication-system work", this);
    auto* pharmacyLayout = new QVBoxLayout(pharmacyGroup);
    auto* pharmacyTable = new QTableWidget(this);
    const QStringList pharmacyCols{"id","review_date", "resident_name", "item_name", "owner", "priority", "status", "notes"};
    pharmacyTable->setColumnCount(pharmacyCols.size());
    pharmacyTable->setHorizontalHeaderLabels({"ID","Date", "Resident", "Item", "Owner", "Priority", "Status", "Notes"});
    pharmacyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pharmacyTable->setColumnHidden(0, true);
    pharmacyLayout->addWidget(pharmacyTable);

    auto* pharmacyForm = new QHBoxLayout();
    auto* dateEdit = new QLineEdit(this);
    dateEdit->setPlaceholderText("YYYY-MM-DD");
    auto* residentName = new QLineEdit(this);
    residentName->setPlaceholderText("Resident");
    auto* itemName = new QLineEdit(this);
    itemName->setPlaceholderText("Pharmacy / med-record item");
    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Owner");
    auto* priority = new QLineEdit(this);
    priority->setPlaceholderText("Priority");
    auto* notes = new QLineEdit(this);
    notes->setPlaceholderText("Notes");
    auto* pharmacyAdd = new QPushButton("Add", this);
    auto* pharmacyLoad = new QPushButton("Load Selected", this);
    auto* pharmacyUpdate = new QPushButton("Update Selected", this);
    auto* pharmacyDelete = new QPushButton("Delete Selected", this);
    pharmacyForm->addWidget(dateEdit);
    pharmacyForm->addWidget(residentName);
    pharmacyForm->addWidget(itemName);
    pharmacyForm->addWidget(owner);
    pharmacyForm->addWidget(priority);
    pharmacyForm->addWidget(notes);
    pharmacyForm->addWidget(pharmacyAdd);
    pharmacyForm->addWidget(pharmacyLoad);
    pharmacyForm->addWidget(pharmacyUpdate);
    pharmacyForm->addWidget(pharmacyDelete);
    pharmacyLayout->addLayout(pharmacyForm);
    root->addWidget(pharmacyGroup);

    auto* infectionGroup = new QGroupBox("Infection-control record work", this);
    auto* infectionLayout = new QVBoxLayout(infectionGroup);
    auto* infectionTable = new QTableWidget(this);
    const QStringList infectionCols{"item_name", "owner", "status", "notes"};
    infectionTable->setColumnCount(infectionCols.size());
    infectionTable->setHorizontalHeaderLabels({"Item", "Owner", "Status", "Notes"});
    infectionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    infectionLayout->addWidget(infectionTable);
    root->addWidget(infectionGroup);

    auto* outbreakGroup = new QGroupBox("Outbreak command visibility", this);
    auto* outbreakLayout = new QVBoxLayout(outbreakGroup);
    auto* outbreakTable = new QTableWidget(this);
    const QStringList outbreakCols{"issue_name", "location_name", "case_count", "owner", "priority", "review_date", "status"};
    outbreakTable->setColumnCount(outbreakCols.size());
    outbreakTable->setHorizontalHeaderLabels({"Issue", "Location", "Cases", "Owner", "Priority", "Review Date", "Status"});
    outbreakTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    outbreakLayout->addWidget(outbreakTable);
    root->addWidget(outbreakGroup);

    auto* docsGroup = new QGroupBox("Medical-record document intake", this);
    auto* docsLayout = new QVBoxLayout(docsGroup);
    auto* docsTable = new QTableWidget(this);
    const QStringList docCols{"doc_date", "doc_name", "owner", "status", "notes"};
    docsTable->setColumnCount(docCols.size());
    docsTable->setHorizontalHeaderLabels({"Date", "Document", "Owner", "Status", "Notes"});
    docsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    docsLayout->addWidget(docsTable);
    root->addWidget(docsGroup, 1);

    auto refreshTables = [=]() {
        pharmacyTable->setRowCount(0);
        for (const auto& row : db->fetchTable("pharmacy_items", pharmacyCols)) {
            const int r = pharmacyTable->rowCount();
            pharmacyTable->insertRow(r);
            for (int c = 0; c < pharmacyCols.size(); ++c) pharmacyTable->setItem(r, c, new QTableWidgetItem(row.value(pharmacyCols[c])));
        }

        infectionTable->setRowCount(0);
        for (const auto& row : db->fetchTable("infection_control_items", infectionCols)) {
            const int r = infectionTable->rowCount();
            infectionTable->insertRow(r);
            for (int c = 0; c < infectionCols.size(); ++c) infectionTable->setItem(r, c, new QTableWidgetItem(row.value(infectionCols[c])));
        }

        outbreakTable->setRowCount(0);
        for (const auto& row : db->fetchTable("outbreak_items", outbreakCols)) {
            const int r = outbreakTable->rowCount();
            outbreakTable->insertRow(r);
            for (int c = 0; c < outbreakCols.size(); ++c) outbreakTable->setItem(r, c, new QTableWidgetItem(row.value(outbreakCols[c])));
        }

        docsTable->setRowCount(0);
        for (const auto& row : db->fetchTable("document_center_items", docCols)) {
            const int r = docsTable->rowCount();
            docsTable->insertRow(r);
            for (int c = 0; c < docCols.size(); ++c) docsTable->setItem(r, c, new QTableWidgetItem(row.value(docCols[c])));
        }
    };

    QObject::connect(pharmacyAdd, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{{"review_date", dateEdit->text().isEmpty() ? "2026-04-20" : dateEdit->text()}, {"resident_name", residentName->text()}, {"item_name", itemName->text()}, {"owner", owner->text()}, {"priority", priority->text().isEmpty() ? "Medium" : priority->text()}, {"status", "Open"}, {"notes", notes->text()}};
        if (db->addRecord("pharmacy_items", values)) {
            dateEdit->clear(); residentName->clear(); itemName->clear(); owner->clear(); priority->clear(); notes->clear();
            refreshTables();
        }
    });
    QObject::connect(pharmacyLoad, &QPushButton::clicked, this, [=]() {
        int row=pharmacyTable->currentRow(); if(row<0) return;
        dateEdit->setText(pharmacyTable->item(row,1)->text()); residentName->setText(pharmacyTable->item(row,2)->text()); itemName->setText(pharmacyTable->item(row,3)->text()); owner->setText(pharmacyTable->item(row,4)->text()); priority->setText(pharmacyTable->item(row,5)->text()); notes->setText(pharmacyTable->item(row,7)->text());
    });
    QObject::connect(pharmacyUpdate, &QPushButton::clicked, this, [=]() {
        int row=pharmacyTable->currentRow(); if(row<0) return; bool ok=false; int id=pharmacyTable->item(row,0)->text().toInt(&ok); if(!ok) return;
        QMap<QString, QString> values{{"review_date", dateEdit->text()}, {"resident_name", residentName->text()}, {"item_name", itemName->text()}, {"owner", owner->text()}, {"priority", priority->text()}, {"notes", notes->text()}};
        if (db->updateRecordById("pharmacy_items", id, values)) refreshTables();
    });
    QObject::connect(pharmacyDelete, &QPushButton::clicked, this, [=]() {
        int row=pharmacyTable->currentRow(); if(row<0) return; bool ok=false; int id=pharmacyTable->item(row,0)->text().toInt(&ok); if(ok && db->deleteRecordById("pharmacy_items", id)) refreshTables();
    });

    refreshTables();
}
