#include "HousekeepingLaundryPage.h"
#include "../../data/DatabaseManager.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

HousekeepingLaundryPage::HousekeepingLaundryPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Housekeeping / Laundry Operations", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Track room-clean turnaround, linen shortages, isolation-laundry follow-up, environmental-services workload, and laundry/housekeeping barriers from one simple operations page.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* tableWidget = new QTableWidget(this);
    const QStringList cols{
        "review_date",
        "area_name",
        "focus_area",
        "item_name",
        "owner",
        "status",
        "notes"
    };

    tableWidget->setColumnCount(cols.size());
    tableWidget->setHorizontalHeaderLabels({
        "Date", "Area", "Focus Area", "Item", "Owner", "Status", "Notes"
    });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshTable = [db, tableWidget, cols]() {
        tableWidget->setRowCount(0);
        const auto rows = db->fetchTable("housekeeping_laundry_items", cols);
        for (const auto& row : rows) {
            const int r = tableWidget->rowCount();
            tableWidget->insertRow(r);
            for (int c = 0; c < cols.size(); ++c) {
                tableWidget->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
            }
        }
    };

    auto* form = new QHBoxLayout();
    auto* dateEdit = new QLineEdit(this);
    dateEdit->setPlaceholderText("YYYY-MM-DD");
    auto* areaName = new QLineEdit(this);
    areaName->setPlaceholderText("Unit / Area");
    auto* focusArea = new QLineEdit(this);
    focusArea->setPlaceholderText("Housekeeping / Laundry / Isolation");
    auto* itemName = new QLineEdit(this);
    itemName->setPlaceholderText("Work item");
    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Owner");
    auto* notes = new QLineEdit(this);
    notes->setPlaceholderText("Notes");
    auto* button = new QPushButton("Add Housekeeping Item", this);

    form->addWidget(dateEdit);
    form->addWidget(areaName);
    form->addWidget(focusArea);
    form->addWidget(itemName);
    form->addWidget(owner);
    form->addWidget(notes);
    form->addWidget(button);

    QObject::connect(button, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values;
        values["review_date"] = dateEdit->text().trimmed();
        values["area_name"] = areaName->text().trimmed();
        values["focus_area"] = focusArea->text().trimmed();
        values["item_name"] = itemName->text().trimmed();
        values["owner"] = owner->text().trimmed();
        values["status"] = "Open";
        values["notes"] = notes->text().trimmed();

        if (db->addRecord("housekeeping_laundry_items", values)) {
            dateEdit->clear();
            areaName->clear();
            focusArea->clear();
            itemName->clear();
            owner->clear();
            notes->clear();
            refreshTable();
        }
    });

    root->addLayout(form);
    root->addWidget(tableWidget, 1);

    refreshTable();
}
