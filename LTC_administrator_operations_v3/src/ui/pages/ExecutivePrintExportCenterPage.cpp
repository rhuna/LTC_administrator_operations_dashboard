#include "ExecutivePrintExportCenterPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QColor>
#include <QComboBox>
#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

ExecutivePrintExportCenterPage::ExecutivePrintExportCenterPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Executive Print & Export Center", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "v80 adds an executive print/export workspace so leadership can queue huddle sheets, survey packets, tracer summaries, plan-of-correction binders, and end-of-day briefing packets in one place before printing, exporting, or handing them off.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet(
        "background:#eef4f8; border:1px solid #d9e2ec; border-radius:10px;"
        "padding:8px 14px; color:#334e68; font-weight:600;");

    root->addWidget(heading);
    root->addWidget(subheading);
    root->addWidget(m_summaryLabel);

    auto* strip = new QHBoxLayout();
    auto makeStat = [&](const QString& title, QLabel*& valueLabel, const QString& hintText) {
        auto* card = new QGroupBox(title, this);
        auto* layout = new QVBoxLayout(card);
        valueLabel = new QLabel("0", card);
        valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #12344d;");
        auto* hint = new QLabel(hintText, card);
        hint->setWordWrap(true);
        hint->setStyleSheet("color:#5b6472;");
        layout->addWidget(valueLabel);
        layout->addWidget(hint);
        strip->addWidget(card);
    };
    makeStat("Open packets", m_openLabel, "Packets still being built, reviewed, or waiting on final content before print/export.");
    makeStat("Due now", m_dueLabel, "Packets leadership should finish today for survey, meeting, or huddle use.");
    makeStat("Ready", m_readyLabel, "Packets marked ready to print or export right now.");
    makeStat("Exported / printed", m_exportedLabel, "Packets already exported, printed, or delivered.");
    root->addLayout(strip);

    auto* formCard = new QGroupBox("Queue executive packet", this);
    auto* formLayout = new QFormLayout(formCard);
    m_packDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_packetNameEdit = new QLineEdit(formCard);
    m_audienceEdit = new QLineEdit(formCard);
    m_scopeEdit = new QLineEdit(formCard);
    m_ownerEdit = new QLineEdit(formCard);
    m_dueDateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"), formCard);
    m_formatCombo = new QComboBox(formCard);
    m_formatCombo->addItems({"PDF Packet", "Print Packet", "Briefing Sheet", "CSV Export", "JSON Snapshot"});
    m_statusCombo = new QComboBox(formCard);
    m_statusCombo->addItems({"Drafting", "Waiting on Input", "Ready", "Exported", "Printed", "Delivered"});
    m_noteEdit = new QTextEdit(formCard);
    m_noteEdit->setPlaceholderText("What should be included, which board data matters most, and whether this packet is for surveyors, leadership, or a huddle.");
    m_noteEdit->setFixedHeight(84);

    formLayout->addRow("Packet date", m_packDateEdit);
    formLayout->addRow("Packet / export name", m_packetNameEdit);
    formLayout->addRow("Audience", m_audienceEdit);
    formLayout->addRow("Content scope", m_scopeEdit);
    formLayout->addRow("Owner", m_ownerEdit);
    formLayout->addRow("Due date", m_dueDateEdit);
    formLayout->addRow("Format", m_formatCombo);
    formLayout->addRow("Status", m_statusCombo);
    formLayout->addRow("Notes", m_noteEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save packet", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* tableCard = new QGroupBox("Executive packet board", this);
    auto* tableLayout = new QVBoxLayout(tableCard);
    m_table = new QTableWidget(tableCard);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels(QStringList{
        "Packet Date", "Packet", "Audience", "Scope", "Owner", "Due Date", "Format", "Status"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    tableLayout->addWidget(m_table);
    root->addWidget(tableCard, 1);

    auto* actionBar = new QHBoxLayout();
    m_readyButton = new QPushButton("Mark Ready", this);
    m_exportedButton = new QPushButton("Mark Exported", this);
    m_exportSnapshotButton = new QPushButton("Export JSON Snapshot", this);
    m_deleteButton = new QPushButton("Delete Selected", this);
    m_deleteButton->setStyleSheet("background:#c0392b;");
    m_refreshButton = new QPushButton("Refresh Board", this);
    actionBar->addWidget(m_readyButton);
    actionBar->addWidget(m_exportedButton);
    actionBar->addWidget(m_exportSnapshotButton);
    actionBar->addWidget(m_deleteButton);
    actionBar->addStretch();
    actionBar->addWidget(m_refreshButton);
    root->addLayout(actionBar);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &ExecutivePrintExportCenterPage::addPacket);
    QObject::connect(m_readyButton, &QPushButton::clicked, this, &ExecutivePrintExportCenterPage::markSelectedReady);
    QObject::connect(m_exportedButton, &QPushButton::clicked, this, &ExecutivePrintExportCenterPage::markSelectedExported);
    QObject::connect(m_exportSnapshotButton, &QPushButton::clicked, this, &ExecutivePrintExportCenterPage::exportSnapshot);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &ExecutivePrintExportCenterPage::deleteSelected);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &ExecutivePrintExportCenterPage::refreshBoard);

    refreshBoard();
}

void ExecutivePrintExportCenterPage::addPacket() {
    if (!m_db) return;
    const QString packetName = m_packetNameEdit->text().trimmed();
    const QString audience = m_audienceEdit->text().trimmed();
    const QString scope = m_scopeEdit->text().trimmed();
    if (packetName.isEmpty() || audience.isEmpty() || scope.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Packet name, audience, and content scope are required.");
        return;
    }

    if (!m_db->addRecord("executive_export_packets", {
            {"pack_date", m_packDateEdit->text().trimmed()},
            {"packet_name", packetName},
            {"audience_name", audience},
            {"content_scope", scope},
            {"owner_name", m_ownerEdit->text().trimmed()},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"format_name", m_formatCombo->currentText()},
            {"status", m_statusCombo->currentText()},
            {"notes", m_noteEdit->toPlainText().trimmed()}
        })) {
        QMessageBox::warning(this, "Save failed", "The executive packet could not be saved.");
        return;
    }

    m_packetNameEdit->clear();
    m_audienceEdit->clear();
    m_scopeEdit->clear();
    m_ownerEdit->clear();
    m_dueDateEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_formatCombo->setCurrentText("PDF Packet");
    m_statusCombo->setCurrentText("Drafting");
    m_noteEdit->clear();
    refreshBoard();
}

void ExecutivePrintExportCenterPage::markSelectedReady() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a packet first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("executive_export_packets", id, {{"status", "Ready"}});
    refreshBoard();
}

void ExecutivePrintExportCenterPage::markSelectedExported() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a packet first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    m_db->updateRecordById("executive_export_packets", id, {{"status", "Exported"}});
    refreshBoard();
}

void ExecutivePrintExportCenterPage::deleteSelected() {
    if (!m_db) return;
    const int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No selection", "Select a packet first.");
        return;
    }
    const int id = m_table->item(row, 0) ? m_table->item(row, 0)->data(Qt::UserRole).toInt() : -1;
    if (id < 0) return;
    if (QMessageBox::question(this, "Confirm delete", "Delete this executive packet entry?") == QMessageBox::Yes) {
        m_db->deleteRecordById("executive_export_packets", id);
        refreshBoard();
    }
}

void ExecutivePrintExportCenterPage::exportSnapshot() {
    if (!m_db) return;
    QString path;
    if (!m_db->exportJsonSnapshot(&path)) {
        QMessageBox::warning(this, "Export failed", "The JSON snapshot could not be created.");
        return;
    }
    QMessageBox::information(this, "Snapshot created", QString("JSON snapshot exported to:\n%1").arg(path));
}

void ExecutivePrintExportCenterPage::refreshBoard() {
    if (!m_db) return;

    const auto rows = m_db->fetchTable(
        "executive_export_packets",
        {"id", "pack_date", "packet_name", "audience_name", "content_scope", "owner_name", "due_date", "format_name", "status", "notes"});

    const QString today = QDate::currentDate().toString("yyyy-MM-dd");
    const QString openFilter = "status='Drafting' OR status='Waiting on Input' OR status='Ready'";
    const int openCount = m_db->countWhere("executive_export_packets", openFilter);
    const int dueCount = m_db->countWhere("executive_export_packets", QString("(%1) AND due_date <= '%2'").arg(openFilter, today));
    const int readyCount = m_db->countWhere("executive_export_packets", "status='Ready'");
    const int exportedCount = m_db->countWhere("executive_export_packets", "status='Exported' OR status='Printed' OR status='Delivered'");

    m_openLabel->setText(QString::number(openCount));
    m_dueLabel->setText(QString::number(dueCount));
    m_readyLabel->setText(QString::number(readyCount));
    m_exportedLabel->setText(QString::number(exportedCount));

    m_summaryLabel->setText(
        QString("%1 open packet(s)  ·  %2 due now  ·  %3 ready  ·  %4 exported/printed  ·  %5 live survey request(s) and %6 document pull(s) remain active alongside packet prep")
            .arg(openCount)
            .arg(dueCount)
            .arg(readyCount)
            .arg(exportedCount)
            .arg(m_db->countWhere("survey_live_requests", "status='Open' OR status='Assigned' OR status='Gathering' OR status='Due Soon'"))
            .arg(m_db->countWhere("survey_document_requests", "status='Open' OR status='Locating' OR status='Printing' OR status='Ready to Deliver' OR status='Missing'")));

    m_table->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_table->rowCount();
        m_table->insertRow(r);

        auto* dateItem = new QTableWidgetItem(row.value("pack_date"));
        dateItem->setData(Qt::UserRole, row.value("id").toInt());
        auto* packetItem = new QTableWidgetItem(row.value("packet_name"));
        auto* audienceItem = new QTableWidgetItem(row.value("audience_name"));
        auto* scopeItem = new QTableWidgetItem(row.value("content_scope"));
        auto* ownerItem = new QTableWidgetItem(row.value("owner_name"));
        auto* dueItem = new QTableWidgetItem(row.value("due_date"));
        auto* formatItem = new QTableWidgetItem(row.value("format_name"));
        auto* statusItem = new QTableWidgetItem(row.value("status"));

        const QString status = row.value("status");
        if (status == "Ready") statusItem->setForeground(QColor("#0b4f8a"));
        else if (status == "Exported" || status == "Printed" || status == "Delivered") statusItem->setForeground(QColor("#166534"));
        else if (status == "Waiting on Input") statusItem->setForeground(QColor("#92400e"));

        const QString notes = row.value("notes").trimmed();
        if (!notes.isEmpty()) {
            packetItem->setToolTip(notes);
            scopeItem->setToolTip(notes);
        }

        m_table->setItem(r, 0, dateItem);
        m_table->setItem(r, 1, packetItem);
        m_table->setItem(r, 2, audienceItem);
        m_table->setItem(r, 3, scopeItem);
        m_table->setItem(r, 4, ownerItem);
        m_table->setItem(r, 5, dueItem);
        m_table->setItem(r, 6, formatItem);
        m_table->setItem(r, 7, statusItem);
    }
}
