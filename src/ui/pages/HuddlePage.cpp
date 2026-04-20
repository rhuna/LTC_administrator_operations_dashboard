#include "HuddlePage.h"

#include <QComboBox>
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

HuddlePage::HuddlePage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      table_(new QTableView(this)),
      model_(nullptr),
      huddleDateEdit_(new QDateEdit(QDate::currentDate(), this)),
      shiftCombo_(new QComboBox(this)),
      departmentEdit_(new QLineEdit(this)),
      topicEdit_(new QLineEdit(this)),
      ownerEdit_(new QLineEdit(this)),
      priorityCombo_(new QComboBox(this)),
      statusCombo_(new QComboBox(this)),
      notesEdit_(new QPlainTextEdit(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Daily Operations Huddle", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* box = new QGroupBox("Add Huddle Follow-Up Item", this);
    auto* form = new QFormLayout(box);

    huddleDateEdit_->setCalendarPopup(true);
    shiftCombo_->addItems({"Morning", "Midday", "Evening", "Weekend"});
    priorityCombo_->addItems({"Low", "Medium", "High", "Critical"});
    statusCombo_->addItems({"Open", "In Progress", "Escalated", "Closed"});
    notesEdit_->setPlaceholderText("Capture the operational issue, resident-care risk, and the follow-up expected from leadership.");
    notesEdit_->setMinimumHeight(90);

    form->addRow("Huddle Date", huddleDateEdit_);
    form->addRow("Shift", shiftCombo_);
    form->addRow("Department", departmentEdit_);
    form->addRow("Topic", topicEdit_);
    form->addRow("Owner", ownerEdit_);
    form->addRow("Priority", priorityCombo_);
    form->addRow("Status", statusCombo_);
    form->addRow("Notes", notesEdit_);

    auto* addButton = new QPushButton("Add Huddle Item", box);
    connect(addButton, &QPushButton::clicked, this, &HuddlePage::onAddHuddleItemClicked);
    form->addRow(addButton);

    root->addWidget(box);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void HuddlePage::refresh() {
    delete model_;
    model_ = db_->createHuddleItemsModel(this);
    table_->setModel(model_);
}

void HuddlePage::onAddHuddleItemClicked() {
    if (departmentEdit_->text().trimmed().isEmpty() ||
        topicEdit_->text().trimmed().isEmpty() ||
        ownerEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete the department, topic, and owner fields.");
        return;
    }

    if (!db_->addHuddleItem(huddleDateEdit_->date().toString("yyyy-MM-dd"),
                            shiftCombo_->currentText(),
                            departmentEdit_->text().trimmed(),
                            topicEdit_->text().trimmed(),
                            ownerEdit_->text().trimmed(),
                            priorityCombo_->currentText(),
                            statusCombo_->currentText(),
                            notesEdit_->toPlainText().trimmed())) {
        QMessageBox::critical(this, "Save Failed", "The huddle item could not be saved to the database.");
        return;
    }

    departmentEdit_->clear();
    topicEdit_->clear();
    ownerEdit_->clear();
    priorityCombo_->setCurrentIndex(1);
    statusCombo_->setCurrentIndex(0);
    notesEdit_->clear();
    huddleDateEdit_->setDate(QDate::currentDate());
    refresh();
}
