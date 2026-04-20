#include "QapiPage.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
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

QapiPage::QapiPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      topicEdit_(new QLineEdit(this)),
      ownerEdit_(new QLineEdit(this)),
      sourceCombo_(new QComboBox(this)),
      stageCombo_(new QComboBox(this)),
      reviewDateEdit_(new QDateEdit(QDate::currentDate(), this)),
      aimEdit_(new QPlainTextEdit(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("QAPI and PIP Workspace", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Open or Track a Performance Improvement Project", this);
    auto* form = new QFormLayout(formBox);

    sourceCombo_->addItems({"Survey Trend", "Incident Review", "Clinical Audit", "Staffing", "Resident Concern", "Readmission"});
    stageCombo_->addItems({"Charter", "Data Collection", "Root Cause", "Intervention", "Sustainment"});
    reviewDateEdit_->setCalendarPopup(true);
    aimEdit_->setPlaceholderText("State the problem, goal, and what success should look like.");
    aimEdit_->setFixedHeight(90);

    form->addRow("Project Topic", topicEdit_);
    form->addRow("Owner", ownerEdit_);
    form->addRow("Trigger Source", sourceCombo_);
    form->addRow("Current Stage", stageCombo_);
    form->addRow("Next Review", reviewDateEdit_);
    form->addRow("Aim Statement", aimEdit_);

    auto* addButton = new QPushButton("Add QAPI/PIP Project", formBox);
    connect(addButton, &QPushButton::clicked, this, &QapiPage::onAddProjectClicked);
    form->addRow(addButton);
    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void QapiPage::refresh() {
    delete model_;
    model_ = db_->createQapiModel(this);
    table_->setModel(model_);
}

void QapiPage::onAddProjectClicked() {
    if (topicEdit_->text().trimmed().isEmpty() || ownerEdit_->text().trimmed().isEmpty() || aimEdit_->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete topic, owner, and aim statement fields.");
        return;
    }

    if (!db_->addQapiProject(topicEdit_->text().trimmed(),
                             ownerEdit_->text().trimmed(),
                             sourceCombo_->currentText(),
                             stageCombo_->currentText(),
                             reviewDateEdit_->date().toString("yyyy-MM-dd"),
                             aimEdit_->toPlainText().trimmed())) {
        QMessageBox::critical(this, "Save Failed", "The QAPI/PIP project could not be saved.");
        return;
    }

    topicEdit_->clear();
    ownerEdit_->clear();
    sourceCombo_->setCurrentIndex(0);
    stageCombo_->setCurrentIndex(0);
    reviewDateEdit_->setDate(QDate::currentDate());
    aimEdit_->clear();
    refresh();
}
