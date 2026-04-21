#include "DietaryPage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

DietaryPage::DietaryPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Dietary / Nutrition Operations", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Track nutrition-risk follow-up, meal-service issues, weight-loss watch items, supplements, and tray-service coordination from one simple workspace.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* tableWidget = new QTableWidget(this);
    const QStringList cols{
        "review_date",
        "resident_name",
        "item_name",
        "owner",
        "priority",
        "status",
        "notes"
    };

    tableWidget->setColumnCount(cols.size());
    tableWidget->setHorizontalHeaderLabels({
        "Date", "Resident", "Item", "Owner", "Priority", "Status", "Notes"
    });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshTable = [db, tableWidget, cols]() {
        tableWidget->setRowCount(0);
        const auto rows = db->fetchTable("dietary_items", cols);
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
    auto* residentName = new QLineEdit(this);
    residentName->setPlaceholderText("Resident");
    auto* itemName = new QLineEdit(this);
    itemName->setPlaceholderText("Dietary / nutrition item");
    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Owner");
    auto* priority = new QLineEdit(this);
    priority->setPlaceholderText("Priority");
    auto* notes = new QLineEdit(this);
    notes->setPlaceholderText("Notes");
    auto* button = new QPushButton("Add Dietary Item", this);

    form->addWidget(dateEdit);
    form->addWidget(residentName);
    form->addWidget(itemName);
    form->addWidget(owner);
    form->addWidget(priority);
    form->addWidget(notes);
    form->addWidget(button);

    root->addLayout(form);
    root->addWidget(tableWidget);

    QObject::connect(button, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{
            {"review_date", dateEdit->text().isEmpty() ? "2026-04-20" : dateEdit->text()},
            {"resident_name", residentName->text()},
            {"item_name", itemName->text()},
            {"owner", owner->text()},
            {"priority", priority->text().isEmpty() ? "Medium" : priority->text()},
            {"status", "Open"},
            {"notes", notes->text()}
        };
        db->addRecord("dietary_items", values);
        refreshTable();
        dateEdit->clear();
        residentName->clear();
        itemName->clear();
        owner->clear();
        priority->clear();
        notes->clear();
    });

    refreshTable();
}
