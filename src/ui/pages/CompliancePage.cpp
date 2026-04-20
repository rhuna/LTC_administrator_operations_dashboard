#include "CompliancePage.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>
#include <QVBoxLayout>

#include "data/DatabaseManager.h"

CompliancePage::CompliancePage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      areaEdit_(new QLineEdit(this)),
      itemEdit_(new QLineEdit(this)),
      ownerEdit_(new QLineEdit(this)),
      dueDateEdit_(new QDateEdit(QDate::currentDate(), this)),
      statusCombo_(new QComboBox(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Compliance Calendar and License Tracker", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Add Regulatory Deadline or Follow-up", this);
    auto* form = new QFormLayout(formBox);

    dueDateEdit_->setCalendarPopup(true);
    statusCombo_->addItems({"Due Soon", "In Progress", "Complete", "Overdue"});

    form->addRow("Area", areaEdit_);
    form->addRow("Requirement", itemEdit_);
    form->addRow("Owner", ownerEdit_);
    form->addRow("Due Date", dueDateEdit_);
    form->addRow("Status", statusCombo_);

    auto* addButton = new QPushButton("Add Compliance Item", formBox);
    connect(addButton, &QPushButton::clicked, this, &CompliancePage::onAddDeadlineClicked);
    form->addRow(addButton);
    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void CompliancePage::refresh() {
    delete model_;
    model_ = db_->createComplianceModel(this);
    table_->setModel(model_);
}

void CompliancePage::onAddDeadlineClicked() {
    if (areaEdit_->text().trimmed().isEmpty() || itemEdit_->text().trimmed().isEmpty() || ownerEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete the area, requirement, and owner fields.");
        return;
    }

    if (!db_->addComplianceItem(areaEdit_->text().trimmed(),
                                itemEdit_->text().trimmed(),
                                ownerEdit_->text().trimmed(),
                                dueDateEdit_->date().toString("yyyy-MM-dd"),
                                statusCombo_->currentText())) {
        QMessageBox::critical(this, "Save Failed", "The compliance item could not be saved.");
        return;
    }

    areaEdit_->clear();
    itemEdit_->clear();
    ownerEdit_->clear();
    dueDateEdit_->setDate(QDate::currentDate());
    statusCombo_->setCurrentIndex(0);
    refresh();
}
