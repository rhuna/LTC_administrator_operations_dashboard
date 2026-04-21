#include "EnvironmentalRoundsPage.h"
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

EnvironmentalRoundsPage::EnvironmentalRoundsPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Environmental Services / Maintenance / Housekeeping / Laundry", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "This page combines plant operations, maintenance-director follow-up, environmental rounds, housekeeping, and laundry so those facility-support workflows sit under one Environmental Services tab.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* envGroup = new QGroupBox("Maintenance and environmental rounds", this);
    auto* envLayout = new QVBoxLayout(envGroup);
    auto* envTable = new QTableWidget(this);
    const QStringList envCols{"round_date", "area_name", "issue_name", "owner", "priority", "status", "notes"};
    envTable->setColumnCount(envCols.size());
    envTable->setHorizontalHeaderLabels({"Date", "Area", "Issue", "Owner", "Priority", "Status", "Notes"});
    envTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    envLayout->addWidget(envTable);

    auto* envForm = new QHBoxLayout();
    auto* roundDate = new QLineEdit(this);
    roundDate->setPlaceholderText("YYYY-MM-DD");
    auto* areaName = new QLineEdit(this);
    areaName->setPlaceholderText("Area / system");
    auto* issueName = new QLineEdit(this);
    issueName->setPlaceholderText("Issue / observation");
    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Maintenance director / owner");
    auto* notes = new QLineEdit(this);
    notes->setPlaceholderText("Notes");
    auto* envButton = new QPushButton("Add Environmental / Maintenance Item", this);
    envForm->addWidget(roundDate);
    envForm->addWidget(areaName);
    envForm->addWidget(issueName);
    envForm->addWidget(owner);
    envForm->addWidget(notes);
    envForm->addWidget(envButton);
    envLayout->addLayout(envForm);
    root->addWidget(envGroup);

    auto* hkGroup = new QGroupBox("Housekeeping and laundry worklist", this);
    auto* hkLayout = new QVBoxLayout(hkGroup);
    auto* hkTable = new QTableWidget(this);
    const QStringList hkCols{"review_date", "area_name", "focus_area", "item_name", "owner", "status", "notes"};
    hkTable->setColumnCount(hkCols.size());
    hkTable->setHorizontalHeaderLabels({"Date", "Area", "Focus Area", "Item", "Owner", "Status", "Notes"});
    hkTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    hkLayout->addWidget(hkTable);

    auto* hkForm = new QHBoxLayout();
    auto* hkDate = new QLineEdit(this);
    hkDate->setPlaceholderText("YYYY-MM-DD");
    auto* hkArea = new QLineEdit(this);
    hkArea->setPlaceholderText("Room / area");
    auto* hkFocus = new QLineEdit(this);
    hkFocus->setPlaceholderText("Laundry / Housekeeping / Turnover");
    auto* hkItem = new QLineEdit(this);
    hkItem->setPlaceholderText("Item / concern");
    auto* hkOwner = new QLineEdit(this);
    hkOwner->setPlaceholderText("Owner");
    auto* hkNotes = new QLineEdit(this);
    hkNotes->setPlaceholderText("Notes");
    auto* hkButton = new QPushButton("Add Housekeeping / Laundry Item", this);
    hkForm->addWidget(hkDate);
    hkForm->addWidget(hkArea);
    hkForm->addWidget(hkFocus);
    hkForm->addWidget(hkItem);
    hkForm->addWidget(hkOwner);
    hkForm->addWidget(hkNotes);
    hkForm->addWidget(hkButton);
    hkLayout->addLayout(hkForm);
    root->addWidget(hkGroup, 1);

    auto refreshTables = [=]() {
        envTable->setRowCount(0);
        const auto envRows = db->fetchTable("environmental_rounds", envCols);
        for (const auto& row : envRows) {
            const int r = envTable->rowCount();
            envTable->insertRow(r);
            for (int c = 0; c < envCols.size(); ++c) {
                envTable->setItem(r, c, new QTableWidgetItem(row.value(envCols[c])));
            }
        }

        hkTable->setRowCount(0);
        const auto hkRows = db->fetchTable("housekeeping_laundry_items", hkCols);
        for (const auto& row : hkRows) {
            const int r = hkTable->rowCount();
            hkTable->insertRow(r);
            for (int c = 0; c < hkCols.size(); ++c) {
                hkTable->setItem(r, c, new QTableWidgetItem(row.value(hkCols[c])));
            }
        }
    };

    QObject::connect(envButton, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{
            {"round_date", roundDate->text().trimmed()},
            {"area_name", areaName->text().trimmed()},
            {"issue_name", issueName->text().trimmed()},
            {"owner", owner->text().trimmed()},
            {"priority", "Medium"},
            {"status", "Open"},
            {"notes", notes->text().trimmed()}
        };
        if (db->addRecord("environmental_rounds", values)) {
            roundDate->clear();
            areaName->clear();
            issueName->clear();
            owner->clear();
            notes->clear();
            refreshTables();
        }
    });

    QObject::connect(hkButton, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{
            {"review_date", hkDate->text().trimmed()},
            {"area_name", hkArea->text().trimmed()},
            {"focus_area", hkFocus->text().trimmed()},
            {"item_name", hkItem->text().trimmed()},
            {"owner", hkOwner->text().trimmed()},
            {"status", "Open"},
            {"notes", hkNotes->text().trimmed()}
        };
        if (db->addRecord("housekeeping_laundry_items", values)) {
            hkDate->clear();
            hkArea->clear();
            hkFocus->clear();
            hkItem->clear();
            hkOwner->clear();
            hkNotes->clear();
            refreshTables();
        }
    });

    refreshTables();
}
