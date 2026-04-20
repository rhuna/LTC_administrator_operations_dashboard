#include "AdmissionsPage.h"

#include <QComboBox>
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

AdmissionsPage::AdmissionsPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      residentEdit_(new QLineEdit(this)),
      referralEdit_(new QLineEdit(this)),
      payerEdit_(new QLineEdit(this)),
      expectedDateEdit_(new QDateEdit(QDate::currentDate(), this)),
      statusCombo_(new QComboBox(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Admissions and Transition Pipeline", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Add Admission or Transition Item", this);
    auto* form = new QFormLayout(formBox);
    statusCombo_->addItems({"Pending", "Insurance Review", "Hospital Hold", "Planned Discharge"});
    expectedDateEdit_->setCalendarPopup(true);
    form->addRow("Resident", residentEdit_);
    form->addRow("Referral Source", referralEdit_);
    form->addRow("Payer", payerEdit_);
    form->addRow("Expected Date", expectedDateEdit_);
    form->addRow("Status", statusCombo_);

    auto* addButton = new QPushButton("Add Pipeline Item", formBox);
    connect(addButton, &QPushButton::clicked, this, &AdmissionsPage::onAddAdmissionClicked);
    form->addRow(addButton);
    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void AdmissionsPage::refresh() {
    delete model_;
    model_ = db_->createAdmissionsModel(this);
    table_->setModel(model_);
}

void AdmissionsPage::onAddAdmissionClicked() {
    if (residentEdit_->text().trimmed().isEmpty() || referralEdit_->text().trimmed().isEmpty() || payerEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete resident, referral source, and payer fields.");
        return;
    }

    if (!db_->addAdmission(residentEdit_->text().trimmed(),
                           referralEdit_->text().trimmed(),
                           payerEdit_->text().trimmed(),
                           expectedDateEdit_->date().toString("yyyy-MM-dd"),
                           statusCombo_->currentText())) {
        QMessageBox::critical(this, "Save Failed", "The admission pipeline item could not be saved.");
        return;
    }

    residentEdit_->clear();
    referralEdit_->clear();
    payerEdit_->clear();
    expectedDateEdit_->setDate(QDate::currentDate());
    statusCombo_->setCurrentIndex(0);
    refresh();
}
