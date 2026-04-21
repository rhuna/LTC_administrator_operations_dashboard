#include "BedBoardPage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

BedBoardPage::BedBoardPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Bed Board / Room Turnover", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel("Track open beds, pending ready rooms, and turnover tasks tied to admissions and discharges.", this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* tableWidget = new QTableWidget(this);
    const QStringList cols{
        "room_number",
        "bed_status",
        "resident_name",
        "pending_action",
        "owner",
        "status",
        "notes"
    };

    tableWidget->setColumnCount(cols.size());
    tableWidget->setHorizontalHeaderLabels({
        "Room", "Bed Status", "Resident", "Pending Action", "Owner", "Status", "Notes"
    });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshTable = [db, tableWidget, cols]() {
        tableWidget->setRowCount(0);
        const auto rows = db->fetchTable("bed_board", cols);
        for (const auto& row : rows) {
            const int r = tableWidget->rowCount();
            tableWidget->insertRow(r);
            for (int c = 0; c < cols.size(); ++c) {
                tableWidget->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
            }
        }
    };

    auto* form = new QHBoxLayout();
    auto* roomNumber = new QLineEdit(this);
    roomNumber->setPlaceholderText("Room");
    auto* bedStatus = new QLineEdit(this);
    bedStatus->setPlaceholderText("Open / Occupied / Hold");
    auto* residentName = new QLineEdit(this);
    residentName->setPlaceholderText("Resident");
    auto* pendingAction = new QLineEdit(this);
    pendingAction->setPlaceholderText("Pending action");
    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Owner");
    auto* notes = new QLineEdit(this);
    notes->setPlaceholderText("Notes");
    auto* button = new QPushButton("Add Bed Board Item", this);

    form->addWidget(roomNumber);
    form->addWidget(bedStatus);
    form->addWidget(residentName);
    form->addWidget(pendingAction);
    form->addWidget(owner);
    form->addWidget(notes);
    form->addWidget(button);

    root->addLayout(form);
    root->addWidget(tableWidget);

    QObject::connect(button, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{
            {"room_number", roomNumber->text()},
            {"bed_status", bedStatus->text().isEmpty() ? "Open" : bedStatus->text()},
            {"resident_name", residentName->text()},
            {"pending_action", pendingAction->text()},
            {"owner", owner->text()},
            {"status", "Open"},
            {"notes", notes->text()}
        };
        db->addRecord("bed_board", values);
        refreshTable();
        roomNumber->clear();
        bedStatus->clear();
        residentName->clear();
        pendingAction->clear();
        owner->clear();
        notes->clear();
    });

    refreshTable();
}
