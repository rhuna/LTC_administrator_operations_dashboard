#include "ShiftHandoffPage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

ShiftHandoffPage::ShiftHandoffPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Shift Handoff Center", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel("Capture open handoff notes between shifts so staffing, admissions, returns, document gaps, and high-priority follow-up are visible to the next leadership team.", this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* tableWidget = new QTableWidget(this);
    const QStringList cols{"handoff_date","shift_name","department","priority","owner_name","status","handoff_note"};
    tableWidget->setColumnCount(cols.size());
    tableWidget->setHorizontalHeaderLabels({"Date", "Shift", "Department", "Priority", "Owner", "Status", "Handoff Note"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshTable = [db, tableWidget, cols]() {
        tableWidget->setRowCount(0);
        const auto rows = db->fetchTable("shift_handoff_items", cols);
        for (const auto& row : rows) {
            const int r = tableWidget->rowCount();
            tableWidget->insertRow(r);
            for (int c = 0; c < cols.size(); ++c) tableWidget->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
        }
    };

    auto* form = new QHBoxLayout();
    auto* dateEdit = new QLineEdit(this); dateEdit->setPlaceholderText("YYYY-MM-DD");
    auto* shiftEdit = new QLineEdit(this); shiftEdit->setPlaceholderText("Day to Evening");
    auto* deptEdit = new QLineEdit(this); deptEdit->setPlaceholderText("Department");
    auto* priorityEdit = new QLineEdit(this); priorityEdit->setPlaceholderText("Priority");
    auto* ownerEdit = new QLineEdit(this); ownerEdit->setPlaceholderText("Owner");
    auto* noteEdit = new QLineEdit(this); noteEdit->setPlaceholderText("Open handoff note");
    auto* button = new QPushButton("Add Handoff Item", this);
    form->addWidget(dateEdit); form->addWidget(shiftEdit); form->addWidget(deptEdit); form->addWidget(priorityEdit); form->addWidget(ownerEdit); form->addWidget(noteEdit, 1); form->addWidget(button);
    root->addLayout(form);
    root->addWidget(tableWidget);

    QObject::connect(button, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{{"handoff_date", dateEdit->text().isEmpty() ? "2026-04-20" : dateEdit->text()},{"shift_name", shiftEdit->text().isEmpty() ? "Day to Evening" : shiftEdit->text()},{"department", deptEdit->text()},{"priority", priorityEdit->text().isEmpty() ? "Medium" : priorityEdit->text()},{"owner_name", ownerEdit->text()},{"status", "Open"},{"handoff_note", noteEdit->text()}};
        db->addRecord("shift_handoff_items", values);
        refreshTable();
        dateEdit->clear(); shiftEdit->clear(); deptEdit->clear(); priorityEdit->clear(); ownerEdit->clear(); noteEdit->clear();
    });

    refreshTable();
}
