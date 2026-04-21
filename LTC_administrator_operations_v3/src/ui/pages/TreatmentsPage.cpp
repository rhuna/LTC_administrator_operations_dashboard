
#include "TreatmentsPage.h"
#include "../../data/DatabaseManager.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

TreatmentsPage::TreatmentsPage(DatabaseManager* db, QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8,8,8,8);
    root->setSpacing(12);
    auto* heading = new QLabel("Treatments / Wounds", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subtitle = new QLabel("Track wound-related treatments, dressing changes, wound-location follow-up, and other treatment items without duplicating MDS or incident workflows.", this);
    subtitle->setWordWrap(true);
    root->addWidget(heading);
    root->addWidget(subtitle);

    auto* table = new QTableWidget(this);
    const QStringList cols{"id","review_date","resident_name","treatment_type","wound_location","owner","status","notes"};
    table->setColumnCount(cols.size());
    table->setHorizontalHeaderLabels({"ID","Date","Resident","Treatment Type","Wound / Location","Owner","Status","Notes"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    root->addWidget(table);

    auto* form = new QHBoxLayout();
    auto* dateEdit = new QLineEdit(this); dateEdit->setPlaceholderText("YYYY-MM-DD");
    auto* residentEdit = new QLineEdit(this); residentEdit->setPlaceholderText("Resident");
    auto* typeEdit = new QLineEdit(this); typeEdit->setPlaceholderText("Treatment / wound type");
    auto* locationEdit = new QLineEdit(this); locationEdit->setPlaceholderText("Location");
    auto* ownerEdit = new QLineEdit(this); ownerEdit->setPlaceholderText("Owner");
    auto* notesEdit = new QLineEdit(this); notesEdit->setPlaceholderText("Notes");
    auto* addButton = new QPushButton("Add Treatment", this);
    auto* deleteButton = new QPushButton("Delete Selected", this);
    form->addWidget(dateEdit); form->addWidget(residentEdit); form->addWidget(typeEdit); form->addWidget(locationEdit); form->addWidget(ownerEdit); form->addWidget(notesEdit); form->addWidget(addButton); form->addWidget(deleteButton);
    root->addLayout(form);

    auto refresh = [=]() {
        table->setRowCount(0);
        const auto rows = db->fetchTable("treatments_items", cols);
        for (const auto& row : rows) {
            int r = table->rowCount(); table->insertRow(r);
            for (int c = 0; c < cols.size(); ++c) table->setItem(r,c,new QTableWidgetItem(row.value(cols[c])));
        }
    };

    QObject::connect(addButton, &QPushButton::clicked, this, [=]() {
        db->addRecord("treatments_items", {
            {"review_date", dateEdit->text()}, {"resident_name", residentEdit->text()}, {"treatment_type", typeEdit->text()},
            {"wound_location", locationEdit->text()}, {"owner", ownerEdit->text()}, {"status", "Open"}, {"notes", notesEdit->text()}
        });
        refresh();
        dateEdit->clear(); residentEdit->clear(); typeEdit->clear(); locationEdit->clear(); ownerEdit->clear(); notesEdit->clear();
    });
    QObject::connect(deleteButton, &QPushButton::clicked, this, [=]() {
        int row = table->currentRow();
        if (row >= 0 && table->item(row,0)) db->deleteRecordById("treatments_items", table->item(row,0)->text().toInt());
        refresh();
    });
    refresh();
}
