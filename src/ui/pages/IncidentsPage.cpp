#include "IncidentsPage.h"

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
#include <QTextEdit>
#include <QVBoxLayout>

#include "data/DatabaseManager.h"

IncidentsPage::IncidentsPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      dateEdit_(new QDateEdit(QDate::currentDate(), this)),
      residentEdit_(new QLineEdit(this)),
      severityCombo_(new QComboBox(this)),
      descriptionEdit_(new QTextEdit(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Incident Log", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Add Incident", this);
    auto* form = new QFormLayout(formBox);
    severityCombo_->addItems({"Low", "Moderate", "High", "Immediate Jeopardy"});
    dateEdit_->setCalendarPopup(true);
    descriptionEdit_->setMinimumHeight(90);
    form->addRow("Event Date", dateEdit_);
    form->addRow("Resident", residentEdit_);
    form->addRow("Severity", severityCombo_);
    form->addRow("Description", descriptionEdit_);

    auto* addButton = new QPushButton("Add Incident", formBox);
    connect(addButton, &QPushButton::clicked, this, &IncidentsPage::onAddIncidentClicked);
    form->addRow(addButton);
    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void IncidentsPage::refresh() {
    delete model_;
    model_ = db_->createIncidentsModel(this);
    table_->setModel(model_);
}

void IncidentsPage::onAddIncidentClicked() {
    if (residentEdit_->text().trimmed().isEmpty() || descriptionEdit_->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete the resident and description fields.");
        return;
    }

    if (!db_->addIncident(dateEdit_->date().toString("yyyy-MM-dd"),
                          residentEdit_->text().trimmed(),
                          severityCombo_->currentText(),
                          descriptionEdit_->toPlainText().trimmed())) {
        QMessageBox::critical(this, "Save Failed", "The incident could not be saved to the database.");
        return;
    }

    residentEdit_->clear();
    descriptionEdit_->clear();
    severityCombo_->setCurrentIndex(0);
    dateEdit_->setDate(QDate::currentDate());
    refresh();
}
