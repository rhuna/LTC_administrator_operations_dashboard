#include "InfectionControlPage.h"

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

InfectionControlPage::InfectionControlPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      eventDateEdit_(new QDateEdit(QDate::currentDate(), this)),
      areaEdit_(new QLineEdit(this)),
      ownerEdit_(new QLineEdit(this)),
      nextReviewEdit_(new QDateEdit(QDate::currentDate().addDays(7), this)),
      statusCombo_(new QComboBox(this)),
      notesEdit_(new QPlainTextEdit(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Infection Prevention and Control", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Add Infection-Control Follow-up", this);
    auto* form = new QFormLayout(formBox);

    eventDateEdit_->setCalendarPopup(true);
    nextReviewEdit_->setCalendarPopup(true);
    statusCombo_->addItems({"Open", "Watch", "In Progress", "Resolved", "Urgent"});
    notesEdit_->setPlaceholderText("Document line-list review, isolation/PPE coaching, antibiotic stewardship, or outbreak follow-up notes...");
    notesEdit_->setMaximumHeight(90);

    form->addRow("Event Date", eventDateEdit_);
    form->addRow("Area / Topic", areaEdit_);
    form->addRow("Owner", ownerEdit_);
    form->addRow("Next Review", nextReviewEdit_);
    form->addRow("Status", statusCombo_);
    form->addRow("Notes", notesEdit_);

    auto* addButton = new QPushButton("Add Infection-Control Item", formBox);
    connect(addButton, &QPushButton::clicked, this, &InfectionControlPage::onAddInfectionItemClicked);
    form->addRow(addButton);
    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void InfectionControlPage::refresh() {
    delete model_;
    model_ = db_->createInfectionControlModel(this);
    table_->setModel(model_);
}

void InfectionControlPage::onAddInfectionItemClicked() {
    if (areaEdit_->text().trimmed().isEmpty() || ownerEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete the area/topic and owner fields.");
        return;
    }

    if (!db_->addInfectionControlItem(eventDateEdit_->date().toString("yyyy-MM-dd"),
                                      areaEdit_->text().trimmed(),
                                      ownerEdit_->text().trimmed(),
                                      nextReviewEdit_->date().toString("yyyy-MM-dd"),
                                      statusCombo_->currentText(),
                                      notesEdit_->toPlainText().trimmed())) {
        QMessageBox::critical(this, "Save Failed", "The infection-control item could not be saved.");
        return;
    }

    eventDateEdit_->setDate(QDate::currentDate());
    areaEdit_->clear();
    ownerEdit_->clear();
    nextReviewEdit_->setDate(QDate::currentDate().addDays(7));
    statusCombo_->setCurrentIndex(0);
    notesEdit_->clear();
    refresh();
}
