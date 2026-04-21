#include "CareConferencePage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

CareConferencePage::CareConferencePage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Care Conferences / Family Communication", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel("Track upcoming care conferences, family communication touchpoints, payer/facility updates, and resident-specific follow-up in one simple workspace.", this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* tableWidget = new QTableWidget(this);
    const QStringList cols{"conference_date","resident_name","contact_name","conference_type","owner_name","status","notes"};
    tableWidget->setColumnCount(cols.size());
    tableWidget->setHorizontalHeaderLabels({"Date", "Resident", "Contact", "Type", "Owner", "Status", "Notes"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshTable = [db, tableWidget, cols]() {
        tableWidget->setRowCount(0);
        const auto rows = db->fetchTable("care_conference_items", cols);
        for (const auto& row : rows) {
            const int r = tableWidget->rowCount();
            tableWidget->insertRow(r);
            for (int c = 0; c < cols.size(); ++c) tableWidget->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
        }
    };

    auto* form = new QHBoxLayout();
    auto* dateEdit = new QLineEdit(this); dateEdit->setPlaceholderText("YYYY-MM-DD");
    auto* residentEdit = new QLineEdit(this); residentEdit->setPlaceholderText("Resident");
    auto* contactEdit = new QLineEdit(this); contactEdit->setPlaceholderText("Family / Contact");
    auto* typeEdit = new QLineEdit(this); typeEdit->setPlaceholderText("Care Conference Type");
    auto* ownerEdit = new QLineEdit(this); ownerEdit->setPlaceholderText("Owner");
    auto* noteEdit = new QLineEdit(this); noteEdit->setPlaceholderText("Notes");
    auto* button = new QPushButton("Add Conference Item", this);
    form->addWidget(dateEdit); form->addWidget(residentEdit); form->addWidget(contactEdit); form->addWidget(typeEdit); form->addWidget(ownerEdit); form->addWidget(noteEdit, 1); form->addWidget(button);
    root->addLayout(form);
    root->addWidget(tableWidget);

    QObject::connect(button, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{{"conference_date", dateEdit->text().isEmpty() ? "2026-04-20" : dateEdit->text()},{"resident_name", residentEdit->text()},{"contact_name", contactEdit->text()},{"conference_type", typeEdit->text().isEmpty() ? "Quarterly Care Conference" : typeEdit->text()},{"owner_name", ownerEdit->text()},{"status", "Open"},{"notes", noteEdit->text()}};
        db->addRecord("care_conference_items", values);
        refreshTable();
        dateEdit->clear(); residentEdit->clear(); contactEdit->clear(); typeEdit->clear(); ownerEdit->clear(); noteEdit->clear();
    });

    refreshTable();
}
