#include "StaffingPage.h"

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

StaffingPage::StaffingPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent),
      db_(db),
      staffingSnapshotTable_(new QTableView(this)),
      staffingChangesTable_(new QTableView(this)),
      staffingSnapshotModel_(nullptr),
      staffingChangesModel_(nullptr),
      changeDateEdit_(new QDateEdit(QDate::currentDate(), this)),
      departmentEdit_(new QLineEdit(this)),
      shiftCombo_(new QComboBox(this)),
      changeTypeCombo_(new QComboBox(this)),
      positionEdit_(new QLineEdit(this)),
      employeeEdit_(new QLineEdit(this)),
      impactCombo_(new QComboBox(this)),
      statusCombo_(new QComboBox(this)),
      notesEdit_(new QPlainTextEdit(this)) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Staffing Operations and Change Control", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    auto* changeBox = new QGroupBox("Record Staffing Change", this);
    auto* changeForm = new QFormLayout(changeBox);

    changeDateEdit_->setCalendarPopup(true);
    shiftCombo_->addItems({"Day", "Evening", "Night", "Weekend", "PRN"});
    changeTypeCombo_->addItems({"Call Off", "Open Shift", "Agency Coverage", "Schedule Adjustment", "Float Assignment", "Extra Shift"});
    impactCombo_->addItems({"Low", "Medium", "High", "Critical"});
    statusCombo_->addItems({"Open", "Planned", "Confirmed", "Resolved"});
    notesEdit_->setPlaceholderText("Describe why the change is needed, resident-care impact, and follow-up notes.");
    notesEdit_->setMinimumHeight(90);

    changeForm->addRow("Change Date", changeDateEdit_);
    changeForm->addRow("Department", departmentEdit_);
    changeForm->addRow("Shift", shiftCombo_);
    changeForm->addRow("Change Type", changeTypeCombo_);
    changeForm->addRow("Position", positionEdit_);
    changeForm->addRow("Employee / Coverage", employeeEdit_);
    changeForm->addRow("Impact", impactCombo_);
    changeForm->addRow("Status", statusCombo_);
    changeForm->addRow("Notes", notesEdit_);

    auto* addButton = new QPushButton("Add Staffing Change", changeBox);
    connect(addButton, &QPushButton::clicked, this, &StaffingPage::onAddStaffingChangeClicked);
    changeForm->addRow(addButton);

    root->addWidget(changeBox);

    auto configureTable = [](QTableView* table) {
        table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setAlternatingRowColors(true);
    };

    auto* snapshotLabel = new QLabel("Current Staffing Snapshot", this);
    snapshotLabel->setObjectName("SectionTitle");
    root->addWidget(snapshotLabel);
    configureTable(staffingSnapshotTable_);
    root->addWidget(staffingSnapshotTable_, 1);

    auto* changesLabel = new QLabel("Staffing Change Log", this);
    changesLabel->setObjectName("SectionTitle");
    root->addWidget(changesLabel);
    configureTable(staffingChangesTable_);
    root->addWidget(staffingChangesTable_, 1);

    refresh();
}

void StaffingPage::refresh() {
    delete staffingSnapshotModel_;
    staffingSnapshotModel_ = db_->createStaffingModel(this);
    staffingSnapshotTable_->setModel(staffingSnapshotModel_);

    delete staffingChangesModel_;
    staffingChangesModel_ = db_->createStaffingChangesModel(this);
    staffingChangesTable_->setModel(staffingChangesModel_);
}

void StaffingPage::onAddStaffingChangeClicked() {
    if (departmentEdit_->text().trimmed().isEmpty() ||
        positionEdit_->text().trimmed().isEmpty() ||
        employeeEdit_->text().trimmed().isEmpty() ||
        notesEdit_->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Data", "Please complete department, position, employee/coverage, and notes fields.");
        return;
    }

    if (!db_->addStaffingChange(changeDateEdit_->date().toString("yyyy-MM-dd"),
                                departmentEdit_->text().trimmed(),
                                shiftCombo_->currentText(),
                                changeTypeCombo_->currentText(),
                                positionEdit_->text().trimmed(),
                                employeeEdit_->text().trimmed(),
                                impactCombo_->currentText(),
                                statusCombo_->currentText(),
                                notesEdit_->toPlainText().trimmed())) {
        QMessageBox::critical(this, "Save Failed", "The staffing change could not be saved.");
        return;
    }

    changeDateEdit_->setDate(QDate::currentDate());
    departmentEdit_->clear();
    shiftCombo_->setCurrentIndex(0);
    changeTypeCombo_->setCurrentIndex(0);
    positionEdit_->clear();
    employeeEdit_->clear();
    impactCombo_->setCurrentIndex(1);
    statusCombo_->setCurrentIndex(0);
    notesEdit_->clear();
    refresh();
}
