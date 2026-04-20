#include "TasksPage.h"

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

TasksPage::TasksPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      titleEdit_(new QLineEdit(this)),
      ownerEdit_(new QLineEdit(this)),
      priorityCombo_(new QComboBox(this)),
      dueDateEdit_(new QDateEdit(QDate::currentDate(), this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Task Tracker", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* formBox = new QGroupBox("Add New Task", this);
    auto* form = new QFormLayout(formBox);
    priorityCombo_->addItems({"Low", "Medium", "High", "Critical"});
    dueDateEdit_->setCalendarPopup(true);
    form->addRow("Title", titleEdit_);
    form->addRow("Owner", ownerEdit_);
    form->addRow("Priority", priorityCombo_);
    form->addRow("Due Date", dueDateEdit_);

    auto* addButton = new QPushButton("Add Task", formBox);
    connect(addButton, &QPushButton::clicked, this, &TasksPage::onAddTaskClicked);
    form->addRow(addButton);
    root->addWidget(formBox);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void TasksPage::refresh() {
    delete model_;
    model_ = db_->createTasksModel(this);
    table_->setModel(model_);
}

void TasksPage::onAddTaskClicked() {
    if (titleEdit_->text().trimmed().isEmpty() || ownerEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete both the title and owner fields.");
        return;
    }

    if (!db_->addTask(titleEdit_->text().trimmed(),
                      ownerEdit_->text().trimmed(),
                      priorityCombo_->currentText(),
                      dueDateEdit_->date().toString("yyyy-MM-dd"))) {
        QMessageBox::critical(this, "Save Failed", "The task could not be saved to the database.");
        return;
    }

    titleEdit_->clear();
    ownerEdit_->clear();
    priorityCombo_->setCurrentIndex(1);
    dueDateEdit_->setDate(QDate::currentDate());
    refresh();
}
