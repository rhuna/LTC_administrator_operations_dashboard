#include "TrainingPage.h"
#include "../../data/DatabaseManager.h"

#include <QComboBox>
#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

TrainingPage::TrainingPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Workforce Development & Training", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Track orientation, CEU completion, competency renewals, and coaching follow-up "
        "for all staff roles in one workforce development workspace.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    root->addWidget(heading);
    root->addWidget(subheading);

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet(
        "background:#eef4f8; border:1px solid #d9e2ec; border-radius:10px;"
        "padding:8px 14px; color:#334e68; font-weight:600;");
    root->addWidget(m_summaryLabel);

    // Add form
    auto* formCard = new QGroupBox("Add training or credentialing item", this);
    auto* formLayout = new QFormLayout(formCard);
    formCard->setStyleSheet("QGroupBox { font-weight: 600; }");

    auto* areaEdit     = new QLineEdit(formCard);
    auto* employeeEdit = new QLineEdit(formCard);
    auto* roleEdit     = new QLineEdit(formCard);
    auto* dueDateEdit  = new QLineEdit(formCard);
    auto* statusCombo  = new QComboBox(formCard);
    auto* notesEdit    = new QLineEdit(formCard);

    areaEdit->setPlaceholderText("Training area (e.g. Infection Control, Abuse Prevention, CEU)");
    employeeEdit->setPlaceholderText("Employee name");
    roleEdit->setPlaceholderText("Role (e.g. CNA, LPN, RN, Dietary)");
    dueDateEdit->setPlaceholderText("Due date YYYY-MM-DD");
    notesEdit->setPlaceholderText("Orientation, renewal, competency, or coaching notes");
    statusCombo->addItems({"Due Soon", "Scheduled", "In Progress", "Complete", "Overdue"});

    formLayout->addRow("Training area", areaEdit);
    formLayout->addRow("Employee", employeeEdit);
    formLayout->addRow("Role", roleEdit);
    formLayout->addRow("Due date", dueDateEdit);
    formLayout->addRow("Status", statusCombo);
    formLayout->addRow("Notes", notesEdit);

    auto* addButton = new QPushButton("Add Training Item", formCard);
    formLayout->addRow(addButton);
    root->addWidget(formCard);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels(
        {"Training Area", "Employee", "Role", "Due Date", "Status", "Notes"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    // Action bar
    auto* actionBar    = new QHBoxLayout();
    auto* completeBtn  = new QPushButton("Mark Complete", this);
    auto* deleteBtn    = new QPushButton("Delete Selected", this);
    deleteBtn->setStyleSheet("background:#c0392b;");
    auto* refreshBtn   = new QPushButton("Refresh", this);
    actionBar->addWidget(completeBtn);
    actionBar->addWidget(deleteBtn);
    actionBar->addStretch();
    actionBar->addWidget(refreshBtn);
    root->addLayout(actionBar);

    refreshTable();

    connect(addButton, &QPushButton::clicked, this, [=]() {
        const QString area     = areaEdit->text().trimmed();
        const QString employee = employeeEdit->text().trimmed();
        const QString role     = roleEdit->text().trimmed();
        if (area.isEmpty() || employee.isEmpty() || role.isEmpty()) {
            QMessageBox::warning(this, "Missing fields",
                "Training area, employee, and role are all required.");
            return;
        }
        QMap<QString, QString> vals;
        vals["area_name"]    = area;
        vals["employee"]     = employee;
        vals["role"]         = role;
        vals["due_date"]     = dueDateEdit->text().trimmed().isEmpty()
                                   ? QDate::currentDate().toString("yyyy-MM-dd")
                                   : dueDateEdit->text().trimmed();
        vals["status"]       = statusCombo->currentText();
        vals["notes"]        = notesEdit->text().trimmed();
        if (m_db->addRecord("training_items", vals)) {
            areaEdit->clear(); employeeEdit->clear(); roleEdit->clear();
            dueDateEdit->clear(); notesEdit->clear();
            statusCombo->setCurrentIndex(0);
            refreshTable();
        } else {
            QMessageBox::warning(this, "Error", "Could not save the training item.");
        }
    });

    connect(completeBtn, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        m_db->updateRecordById("training_items", id, {{"status", "Complete"}});
        refreshTable();
    });

    connect(deleteBtn, &QPushButton::clicked, this, [=]() {
        const int row = m_table->currentRow();
        if (row < 0) { QMessageBox::information(this, "No selection", "Select a row first."); return; }
        const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
        if (id < 0) return;
        if (QMessageBox::question(this, "Confirm delete", "Delete this training item?") == QMessageBox::Yes) {
            m_db->deleteRecordById("training_items", id);
            refreshTable();
        }
    });

    connect(refreshBtn, &QPushButton::clicked, this, &TrainingPage::refreshTable);
}

void TrainingPage::refreshTable() {
    m_table->setRowCount(0);
    const QStringList cols{"id", "area_name", "employee", "role", "due_date", "status", "notes"};
    const auto rows = m_db->fetchTable("training_items", cols);

    int dueCount      = 0;
    int overdueCount  = 0;
    int completeCount = 0;

    const QStringList displayCols{"area_name", "employee", "role", "due_date", "status", "notes"};
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);
        const QString status = row.value("status");
        if (status == "Due Soon" || status == "Scheduled" || status == "In Progress") ++dueCount;
        if (status == "Overdue") ++overdueCount;
        if (status == "Complete") ++completeCount;

        for (int c = 0; c < displayCols.size(); ++c) {
            auto* item = new QTableWidgetItem(row.value(displayCols[c]));
            if (c == 0) item->setData(Qt::UserRole, row.value("id").toInt());
            if (displayCols[c] == "status") {
                if (status == "Overdue")
                    item->setForeground(QColor("#b91c1c"));
                else if (status == "Complete")
                    item->setForeground(QColor("#166534"));
                else if (status == "Due Soon")
                    item->setForeground(QColor("#92400e"));
            }
            m_table->setItem(r, c, item);
        }
    }

    m_summaryLabel->setText(
        QString("%1 in progress / scheduled  ·  %2 overdue  ·  %3 complete  ·  %4 total items")
            .arg(dueCount).arg(overdueCount).arg(completeCount).arg(rows.size()));
}
