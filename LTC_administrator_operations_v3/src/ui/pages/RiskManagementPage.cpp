#include "RiskManagementPage.h"
#include "../../data/DatabaseManager.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

RiskManagementPage::RiskManagementPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);

    auto* heading = new QLabel("Risk Management / Grievances", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* tableWidget = new QTableWidget(this);
    const QStringList cols{
        "report_date",
        "category",
        "resident_or_family",
        "owner",
        "priority",
        "status",
        "summary"
    };

    tableWidget->setColumnCount(cols.size());
    tableWidget->setHorizontalHeaderLabels(cols);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshTable = [db, tableWidget, cols]() {
        tableWidget->setRowCount(0);
        const auto rows = db->fetchTable("grievances", cols);
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
    auto* category = new QLineEdit(this);
    category->setPlaceholderText("Category");

    auto* contact = new QLineEdit(this);
    contact->setPlaceholderText("Resident / Family");

    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Owner");

    auto* summary = new QLineEdit(this);
    summary->setPlaceholderText("Summary");

    auto* button = new QPushButton("Add Grievance", this);

    form->addWidget(category);
    form->addWidget(contact);
    form->addWidget(owner);
    form->addWidget(summary);
    form->addWidget(button);
    root->addLayout(form);

    QObject::connect(button, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{
            {"report_date", "2026-04-20"},
            {"category", category->text()},
            {"resident_or_family", contact->text()},
            {"owner", owner->text()},
            {"priority", "Medium"},
            {"status", "Open"},
            {"summary", summary->text()}
        };

        db->addRecord("grievances", values);
        refreshTable();

        category->clear();
        contact->clear();
        owner->clear();
        summary->clear();
    });

    root->addWidget(tableWidget);
}
