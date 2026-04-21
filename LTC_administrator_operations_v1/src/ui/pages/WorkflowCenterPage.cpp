#include "WorkflowCenterPage.h"
#include "../../data/DatabaseManager.h"

#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

WorkflowCenterPage::WorkflowCenterPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    m_definitions = {
        {"tasks", "Tasks", "Edit or close operational tasks without opening a separate workflow page.", {"id", "due_date", "owner", "task_name", "priority", "status"}},
        {"incidents", "Incidents", "Update incident follow-up status and core details from one workspace.", {"id", "incident_date", "resident_name", "incident_type", "severity", "status"}},
        {"grievances", "Grievances", "Adjust grievance ownership, priority, status, and summary details.", {"id", "report_date", "category", "resident_or_family", "owner", "priority", "status", "summary"}},
        {"bed_board", "Bed Board", "Manage room-turnover records and archive completed bed-board work.", {"id", "room_number", "bed_status", "resident_name", "pending_action", "owner", "status", "notes"}},
        {"transport_items", "Transportation", "Update outside-appointment coordination, packet readiness, and completion status.", {"id", "appointment_date", "resident_name", "appointment_type", "destination", "transport_mode", "owner", "status", "notes"}},
        {"pharmacy_items", "Pharmacy", "Clean up medication-system follow-up without re-entering records.", {"id", "review_date", "resident_name", "item_name", "owner", "priority", "status", "notes"}},
        {"dietary_items", "Dietary", "Edit and archive nutrition follow-up records in one place.", {"id", "review_date", "resident_name", "item_name", "owner", "priority", "status", "notes"}},
        {"compliance_items", "Compliance", "Adjust due dates and close compliance obligations from a single screen.", {"id", "item_name", "due_date", "owner", "status"}},
        {"huddle_items", "Huddle", "Update huddle ownership and status as items are resolved.", {"id", "huddle_date", "shift_name", "department", "topic", "owner", "priority", "status", "notes"}},
        {"environmental_rounds", "Environmental Rounds", "Edit plant-operations and rounds findings without duplicating entries.", {"id", "round_date", "area_name", "issue_name", "owner", "priority", "status", "notes"}}
    };

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(14);

    auto* heading = new QLabel("Workflow Center", this);
    heading->setStyleSheet("font-size: 24px; font-weight: 700; color:#102a43;");
    auto* subheading = new QLabel(
        "v27 adds a simpler edit/archive/delete workspace so high-volume operational records can be corrected without hunting through every module.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color:#486581; font-size:13px;");

    auto* topCard = new QFrame(this);
    topCard->setStyleSheet("QFrame { background:#ffffff; border:1px solid #d9e2ec; border-radius:18px; }");
    auto* topLayout = new QVBoxLayout(topCard);
    topLayout->setContentsMargins(18, 16, 18, 16);
    topLayout->setSpacing(10);

    auto* selectorRow = new QHBoxLayout();
    auto* selectorLabel = new QLabel("Workflow:", topCard);
    selectorLabel->setStyleSheet("font-weight:600; color:#243b53;");
    m_workflowSelector = new QComboBox(topCard);
    for (const auto& definition : m_definitions) {
        m_workflowSelector->addItem(definition.title);
    }
    selectorRow->addWidget(selectorLabel);
    selectorRow->addWidget(m_workflowSelector, 1);

    topLayout->addWidget(heading);
    topLayout->addWidget(subheading);
    topLayout->addLayout(selectorRow);
    root->addWidget(topCard);

    auto* contentRow = new QHBoxLayout();
    contentRow->setSpacing(14);

    auto* tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background:#ffffff; border:1px solid #d9e2ec; border-radius:18px; }");
    auto* tableLayout = new QVBoxLayout(tableCard);
    tableLayout->setContentsMargins(18, 16, 18, 16);
    tableLayout->setSpacing(10);

    auto* tableTitle = new QLabel("Records", tableCard);
    tableTitle->setStyleSheet("font-size:18px; font-weight:700; color:#102a43;");
    auto* tableHint = new QLabel("Select a row to edit or archive it.", tableCard);
    tableHint->setStyleSheet("color:#486581;");

    m_table = new QTableWidget(tableCard);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);

    tableLayout->addWidget(tableTitle);
    tableLayout->addWidget(tableHint);
    tableLayout->addWidget(m_table, 1);
    contentRow->addWidget(tableCard, 3);

    auto* editorCard = new QFrame(this);
    editorCard->setStyleSheet("QFrame { background:#ffffff; border:1px solid #d9e2ec; border-radius:18px; }");
    auto* editorLayout = new QVBoxLayout(editorCard);
    editorLayout->setContentsMargins(18, 16, 18, 16);
    editorLayout->setSpacing(10);

    auto* editorTitle = new QLabel("Selected Record", editorCard);
    editorTitle->setStyleSheet("font-size:18px; font-weight:700; color:#102a43;");
    auto* editorHint = new QLabel("Use save for corrections, archive for completed items, and delete for records that should be removed entirely.", editorCard);
    editorHint->setWordWrap(true);
    editorHint->setStyleSheet("color:#486581;");

    auto* formHost = new QWidget(editorCard);
    m_editorLayout = new QFormLayout(formHost);
    m_editorLayout->setContentsMargins(0, 0, 0, 0);
    m_editorLayout->setSpacing(10);

    auto* buttonRow = new QHBoxLayout();
    m_saveButton = new QPushButton("Save Changes", editorCard);
    m_archiveButton = new QPushButton("Mark Archived", editorCard);
    m_deleteButton = new QPushButton("Delete", editorCard);
    m_refreshButton = new QPushButton("Refresh", editorCard);
    buttonRow->addWidget(m_saveButton);
    buttonRow->addWidget(m_archiveButton);
    buttonRow->addWidget(m_deleteButton);
    buttonRow->addStretch();
    buttonRow->addWidget(m_refreshButton);

    editorLayout->addWidget(editorTitle);
    editorLayout->addWidget(editorHint);
    editorLayout->addWidget(formHost);
    editorLayout->addStretch();
    editorLayout->addLayout(buttonRow);
    contentRow->addWidget(editorCard, 2);

    root->addLayout(contentRow, 1);

    QObject::connect(m_workflowSelector, &QComboBox::currentTextChanged, this, [this](const QString&) {
        buildEditor();
        refreshTable();
    });
    QObject::connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() { loadSelectedRow(); });
    QObject::connect(m_saveButton, &QPushButton::clicked, this, [this]() { handleSave(); });
    QObject::connect(m_archiveButton, &QPushButton::clicked, this, [this]() { handleArchive(); });
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, [this]() { handleDelete(); });
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, [this]() { refreshTable(); });

    buildEditor();
    refreshTable();
}

WorkflowCenterPage::WorkflowDefinition WorkflowCenterPage::currentDefinition() const {
    const int index = m_workflowSelector ? m_workflowSelector->currentIndex() : 0;
    if (index < 0 || index >= m_definitions.size()) {
        return m_definitions.isEmpty() ? WorkflowDefinition{} : m_definitions.first();
    }
    return m_definitions.at(index);
}

void WorkflowCenterPage::buildEditor() {
    while (m_editorLayout->rowCount() > 0) {
        m_editorLayout->removeRow(0);
    }
    m_editors.clear();

    const auto definition = currentDefinition();
    for (const auto& column : definition.columns) {
        auto* edit = new QLineEdit(this);
        if (column == "id") {
            edit->setReadOnly(true);
            edit->setStyleSheet("background:#f0f4f8; color:#52606d;");
        }
        m_editorLayout->addRow(column + ":", edit);
        m_editors.insert(column, edit);
    }
}

void WorkflowCenterPage::refreshTable() {
    const auto definition = currentDefinition();
    if (!m_db || definition.tableName.isEmpty()) {
        return;
    }

    const auto rows = m_db->fetchTable(definition.tableName, definition.columns);
    m_table->clear();
    m_table->setColumnCount(definition.columns.size());
    m_table->setHorizontalHeaderLabels(definition.columns);
    m_table->setRowCount(rows.size());

    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows.at(r);
        for (int c = 0; c < definition.columns.size(); ++c) {
            const auto& key = definition.columns.at(c);
            m_table->setItem(r, c, new QTableWidgetItem(row.value(key)));
        }
    }

    if (m_table->rowCount() > 0) {
        m_table->selectRow(0);
    } else {
        for (auto* editor : m_editors) {
            editor->clear();
        }
    }
}

void WorkflowCenterPage::loadSelectedRow() {
    const int row = m_table->currentRow();
    if (row < 0) {
        return;
    }
    const auto definition = currentDefinition();
    for (int c = 0; c < definition.columns.size(); ++c) {
        const auto key = definition.columns.at(c);
        auto* editor = m_editors.value(key, nullptr);
        if (!editor) {
            continue;
        }
        auto* item = m_table->item(row, c);
        editor->setText(item ? item->text() : QString());
    }
}

void WorkflowCenterPage::handleSave() {
    const auto definition = currentDefinition();
    if (!m_db || definition.tableName.isEmpty()) {
        return;
    }
    const int id = m_editors.value("id") ? m_editors.value("id")->text().toInt() : -1;
    if (id < 0) {
        QMessageBox::information(this, "Workflow Center", "Select a record before saving changes.");
        return;
    }

    QMap<QString, QString> values;
    for (const auto& column : definition.columns) {
        if (column == "id") {
            continue;
        }
        auto* editor = m_editors.value(column, nullptr);
        if (editor) {
            values.insert(column, editor->text().trimmed());
        }
    }

    if (!m_db->updateRecordById(definition.tableName, id, values)) {
        QMessageBox::warning(this, "Workflow Center", "Save failed for the selected record.");
        return;
    }
    refreshTable();
    QMessageBox::information(this, "Workflow Center", "Record updated.");
}

void WorkflowCenterPage::handleArchive() {
    const auto definition = currentDefinition();
    if (!m_db || definition.tableName.isEmpty()) {
        return;
    }
    const int id = m_editors.value("id") ? m_editors.value("id")->text().toInt() : -1;
    if (id < 0) {
        QMessageBox::information(this, "Workflow Center", "Select a record before archiving it.");
        return;
    }

    if (!m_db->archiveRecordById(definition.tableName, id)) {
        QMessageBox::warning(this, "Workflow Center", "Archive failed for the selected record.");
        return;
    }
    refreshTable();
    QMessageBox::information(this, "Workflow Center", "Record marked Archived.");
}

void WorkflowCenterPage::handleDelete() {
    const auto definition = currentDefinition();
    if (!m_db || definition.tableName.isEmpty()) {
        return;
    }
    const int id = m_editors.value("id") ? m_editors.value("id")->text().toInt() : -1;
    if (id < 0) {
        QMessageBox::information(this, "Workflow Center", "Select a record before deleting it.");
        return;
    }

    const auto response = QMessageBox::question(
        this,
        "Workflow Center",
        "Delete the selected record permanently?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (response != QMessageBox::Yes) {
        return;
    }

    if (!m_db->deleteRecordById(definition.tableName, id)) {
        QMessageBox::warning(this, "Workflow Center", "Delete failed for the selected record.");
        return;
    }
    refreshTable();
    QMessageBox::information(this, "Workflow Center", "Record deleted.");
}
