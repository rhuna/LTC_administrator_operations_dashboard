#include "TrainingPage.h"

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

TrainingPage::TrainingPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      areaEdit_(new QLineEdit(this)),
      employeeEdit_(new QLineEdit(this)),
      roleEdit_(new QLineEdit(this)),
      dueDateEdit_(new QDateEdit(QDate::currentDate(), this)),
      statusCombo_(new QComboBox(this)),
      notesEdit_(new QPlainTextEdit(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Workforce Development and Credentialing", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Add Training or Credentialing Follow-up", this);
    auto* form = new QFormLayout(formBox);

    dueDateEdit_->setCalendarPopup(true);
    statusCombo_->addItems({"Due Soon", "Scheduled", "In Progress", "Complete", "Overdue"});
    notesEdit_->setPlaceholderText("Orientation, CEU, renewal, competency, or coaching notes...");
    notesEdit_->setMaximumHeight(90);

    form->addRow("Training Area", areaEdit_);
    form->addRow("Employee", employeeEdit_);
    form->addRow("Role", roleEdit_);
    form->addRow("Due Date", dueDateEdit_);
    form->addRow("Status", statusCombo_);
    form->addRow("Notes", notesEdit_);

    auto* addButton = new QPushButton("Add Training Item", formBox);
    connect(addButton, &QPushButton::clicked, this, &TrainingPage::onAddTrainingClicked);
    form->addRow(addButton);
    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void TrainingPage::refresh() {
    delete model_;
    model_ = db_->createTrainingModel(this);
    table_->setModel(model_);
}

void TrainingPage::onAddTrainingClicked() {
    if (areaEdit_->text().trimmed().isEmpty() ||
        employeeEdit_->text().trimmed().isEmpty() ||
        roleEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete the training area, employee, and role fields.");
        return;
    }

    if (!db_->addTrainingItem(areaEdit_->text().trimmed(),
                              employeeEdit_->text().trimmed(),
                              roleEdit_->text().trimmed(),
                              dueDateEdit_->date().toString("yyyy-MM-dd"),
                              statusCombo_->currentText(),
                              notesEdit_->toPlainText().trimmed())) {
        QMessageBox::critical(this, "Save Failed", "The training item could not be saved.");
        return;
    }

    areaEdit_->clear();
    employeeEdit_->clear();
    roleEdit_->clear();
    dueDateEdit_->setDate(QDate::currentDate());
    statusCombo_->setCurrentIndex(0);
    notesEdit_->clear();
    refresh();
}
