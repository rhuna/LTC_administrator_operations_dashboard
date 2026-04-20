#include "EmergencyPreparednessPage.h"

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

EmergencyPreparednessPage::EmergencyPreparednessPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      drillDateEdit_(new QDateEdit(QDate::currentDate(), this)),
      scenarioEdit_(new QLineEdit(this)),
      ownerEdit_(new QLineEdit(this)),
      nextDrillEdit_(new QDateEdit(QDate::currentDate().addDays(30), this)),
      statusCombo_(new QComboBox(this)),
      notesEdit_(new QPlainTextEdit(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Emergency Preparedness and Drills", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Add Drill or Preparedness Follow-up", this);
    auto* form = new QFormLayout(formBox);

    drillDateEdit_->setCalendarPopup(true);
    nextDrillEdit_->setCalendarPopup(true);
    statusCombo_->addItems({"Due Soon", "Scheduled", "In Progress", "Complete", "Overdue"});
    notesEdit_->setPlaceholderText("Document drill observations, generator checks, evacuation follow-up, or emergency plan notes...");
    notesEdit_->setMaximumHeight(90);

    form->addRow("Last Drill Date", drillDateEdit_);
    form->addRow("Scenario", scenarioEdit_);
    form->addRow("Owner", ownerEdit_);
    form->addRow("Next Drill", nextDrillEdit_);
    form->addRow("Status", statusCombo_);
    form->addRow("Notes", notesEdit_);

    auto* addButton = new QPushButton("Add Preparedness Item", formBox);
    connect(addButton, &QPushButton::clicked, this, &EmergencyPreparednessPage::onAddPreparednessItemClicked);
    form->addRow(addButton);
    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void EmergencyPreparednessPage::refresh() {
    delete model_;
    model_ = db_->createEmergencyPreparednessModel(this);
    table_->setModel(model_);
}

void EmergencyPreparednessPage::onAddPreparednessItemClicked() {
    if (scenarioEdit_->text().trimmed().isEmpty() ||
        ownerEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete the scenario and owner fields.");
        return;
    }

    if (!db_->addEmergencyPreparednessItem(drillDateEdit_->date().toString("yyyy-MM-dd"),
                                           scenarioEdit_->text().trimmed(),
                                           ownerEdit_->text().trimmed(),
                                           nextDrillEdit_->date().toString("yyyy-MM-dd"),
                                           statusCombo_->currentText(),
                                           notesEdit_->toPlainText().trimmed())) {
        QMessageBox::critical(this, "Save Failed", "The preparedness item could not be saved.");
        return;
    }

    drillDateEdit_->setDate(QDate::currentDate());
    scenarioEdit_->clear();
    ownerEdit_->clear();
    nextDrillEdit_->setDate(QDate::currentDate().addDays(30));
    statusCombo_->setCurrentIndex(0);
    notesEdit_->clear();
    refresh();
}
