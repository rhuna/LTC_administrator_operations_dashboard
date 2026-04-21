#include "EnvironmentalRoundsPage.h"
#include "../../data/DatabaseManager.h"
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

    auto* heading = new QLabel("Environmental Rounds / Plant Operations", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* tableWidget = new QTableWidget(this);
    const QStringList cols{
        "round_date",
        "area_name",
        "issue_name",
        "owner",
        "priority",
        "status",
        "notes"
    };

    tableWidget->setColumnCount(cols.size());
    tableWidget->setHorizontalHeaderLabels(cols);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshTable = [db, tableWidget, cols]() {
        tableWidget->setRowCount(0);
        const auto rows = db->fetchTable("environmental_rounds", cols);
        for (const auto& row : rows) {
            const int r = tableWidget->rowCount();
            tableWidget->insertRow(r);
            for (int c = 0; c < cols.size(); ++c) {
                tableWidget->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
            }
        }
    };

    refreshTable();

    auto* form = new QHBoxLayout();
    auto* areaName = new QLineEdit(this);
    areaName->setPlaceholderText("Area");

    auto* issueName = new QLineEdit(this);
    issueName->setPlaceholderText("Issue / Observation");

    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Owner");

    auto* notes = new QLineEdit(this);
    notes->setPlaceholderText("Notes");

    auto* button = new QPushButton("Add Round Item", this);

    form->addWidget(areaName);
    form->addWidget(issueName);
    form->addWidget(owner);
    form->addWidget(notes);
    form->addWidget(button);
    root->addLayout(form);

    QObject::connect(button, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{
            {"round_date", "2026-04-20"},
            {"area_name", areaName->text()},
            {"issue_name", issueName->text()},
            {"owner", owner->text()},
            {"priority", "Medium"},
            {"status", "Open"},
            {"notes", notes->text()}
        };

        db->addRecord("environmental_rounds", values);
        refreshTable();

        areaName->clear();
        issueName->clear();
        owner->clear();
        notes->clear();
    });

    root->addWidget(tableWidget);
}
