#include "BudgetPage.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
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

BudgetPage::BudgetPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      categoryEdit_(new QLineEdit(this)),
      ownerEdit_(new QLineEdit(this)),
      budgetSpin_(new QDoubleSpinBox(this)),
      actualSpin_(new QDoubleSpinBox(this)),
      statusCombo_(new QComboBox(this)),
      reviewDateEdit_(new QDateEdit(QDate::currentDate(), this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Budget and Labor Variance", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Add Budget Watch Item", this);
    auto* form = new QFormLayout(formBox);

    budgetSpin_->setMaximum(1000000.0);
    budgetSpin_->setPrefix("$");
    budgetSpin_->setDecimals(2);
    actualSpin_->setMaximum(1000000.0);
    actualSpin_->setPrefix("$");
    actualSpin_->setDecimals(2);
    statusCombo_->addItems({"On Target", "Watch", "At Risk"});
    reviewDateEdit_->setCalendarPopup(true);

    form->addRow("Category", categoryEdit_);
    form->addRow("Owner", ownerEdit_);
    form->addRow("Monthly Budget", budgetSpin_);
    form->addRow("Actual Spend", actualSpin_);
    form->addRow("Status", statusCombo_);
    form->addRow("Next Review", reviewDateEdit_);

    auto* addButton = new QPushButton("Add Budget Item", formBox);
    connect(addButton, &QPushButton::clicked, this, &BudgetPage::onAddBudgetItemClicked);
    form->addRow(addButton);
    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void BudgetPage::refresh() {
    delete model_;
    model_ = db_->createBudgetModel(this);
    table_->setModel(model_);
}

void BudgetPage::onAddBudgetItemClicked() {
    if (categoryEdit_->text().trimmed().isEmpty() || ownerEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete the category and owner fields.");
        return;
    }

    if (!db_->addBudgetItem(categoryEdit_->text().trimmed(),
                            ownerEdit_->text().trimmed(),
                            budgetSpin_->value(),
                            actualSpin_->value(),
                            statusCombo_->currentText(),
                            reviewDateEdit_->date().toString("yyyy-MM-dd"))) {
        QMessageBox::critical(this, "Save Failed", "The budget item could not be saved.");
        return;
    }

    categoryEdit_->clear();
    ownerEdit_->clear();
    budgetSpin_->setValue(0.0);
    actualSpin_->setValue(0.0);
    statusCombo_->setCurrentIndex(0);
    reviewDateEdit_->setDate(QDate::currentDate());
    refresh();
}
