#include "QualityMeasuresPage.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>
#include <QVBoxLayout>

#include "data/DatabaseManager.h"

QualityMeasuresPage::QualityMeasuresPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      measureNameEdit_(new QLineEdit(this)),
      ownerEdit_(new QLineEdit(this)),
      measureSetCombo_(new QComboBox(this)),
      currentValueSpin_(new QDoubleSpinBox(this)),
      targetValueSpin_(new QDoubleSpinBox(this)),
      reviewDateEdit_(new QDateEdit(QDate::currentDate(), this)),
      statusCombo_(new QComboBox(this)),
      notesEdit_(new QPlainTextEdit(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Quality Measures and Clinical Outcomes", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Add Quality Measure Watch Item", this);
    auto* form = new QFormLayout(formBox);

    measureSetCombo_->addItems({"CMS Quality Measure", "Internal KPI", "Infection Control", "Rehospitalization", "Clinical Risk"});
    currentValueSpin_->setRange(0.0, 1000.0);
    currentValueSpin_->setDecimals(2);
    targetValueSpin_->setRange(0.0, 1000.0);
    targetValueSpin_->setDecimals(2);
    reviewDateEdit_->setCalendarPopup(true);
    statusCombo_->addItems({"On Target", "Watch", "Off Target"});
    notesEdit_->setPlaceholderText("Describe the trend, likely drivers, and the next intervention or monitoring step.");
    notesEdit_->setMinimumHeight(90);

    form->addRow("Measure", measureNameEdit_);
    form->addRow("Measure Set", measureSetCombo_);
    form->addRow("Owner", ownerEdit_);
    form->addRow("Current Value", currentValueSpin_);
    form->addRow("Target Value", targetValueSpin_);
    form->addRow("Next Review", reviewDateEdit_);
    form->addRow("Status", statusCombo_);
    form->addRow("Notes", notesEdit_);

    auto* addButton = new QPushButton("Add Quality Measure", formBox);
    connect(addButton, &QPushButton::clicked, this, &QualityMeasuresPage::onAddMeasureClicked);
    form->addRow(addButton);

    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void QualityMeasuresPage::refresh() {
    delete model_;
    model_ = db_->createQualityMeasuresModel(this);
    table_->setModel(model_);
}

void QualityMeasuresPage::onAddMeasureClicked() {
    if (measureNameEdit_->text().trimmed().isEmpty() || ownerEdit_->text().trimmed().isEmpty() || notesEdit_->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete the measure, owner, and notes fields.");
        return;
    }

    if (!db_->addQualityMeasure(measureNameEdit_->text().trimmed(),
                                measureSetCombo_->currentText(),
                                ownerEdit_->text().trimmed(),
                                currentValueSpin_->value(),
                                targetValueSpin_->value(),
                                reviewDateEdit_->date().toString("yyyy-MM-dd"),
                                statusCombo_->currentText(),
                                notesEdit_->toPlainText().trimmed())) {
        QMessageBox::critical(this, "Save Failed", "The quality measure could not be saved.");
        return;
    }

    measureNameEdit_->clear();
    ownerEdit_->clear();
    measureSetCombo_->setCurrentIndex(0);
    currentValueSpin_->setValue(0.0);
    targetValueSpin_->setValue(0.0);
    reviewDateEdit_->setDate(QDate::currentDate());
    statusCombo_->setCurrentIndex(0);
    notesEdit_->clear();
    refresh();
}
