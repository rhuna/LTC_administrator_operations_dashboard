#include "AdmissionsPage.h"
#include "../../data/DatabaseManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
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
#include <QVBoxLayout>
#include <QStandardPaths>

AdmissionsPage::AdmissionsPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Admissions Waitlist", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Enter referral waitlist items with payer, diagnosis, MDS details, and target room. Admissions now also shows current room availability so you can choose an open room during admit planning.", this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    root->addWidget(heading);
    root->addWidget(subheading);

    auto* formCard = new QGroupBox("Add referral / waitlist item", this);
    auto* formLayout = new QFormLayout(formCard);
    formCard->setStyleSheet("QGroupBox { font-weight: 600; }");

    m_nameEdit = new QLineEdit(formCard);
    m_sourceEdit = new QLineEdit(formCard);
    m_dateEdit = new QLineEdit(formCard);
    m_statusEdit = new QLineEdit(formCard);
    m_roomEdit = new QLineEdit(formCard);
    m_payerEdit = new QLineEdit(formCard);
    m_diagnosisEdit = new QLineEdit(formCard);
    m_assessmentTypeEdit = new QLineEdit(formCard);
    m_ardDateEdit = new QLineEdit(formCard);
    m_notesEdit = new QLineEdit(formCard);

    m_nameEdit->setPlaceholderText("Resident name");
    m_sourceEdit->setPlaceholderText("Referral source");
    m_dateEdit->setPlaceholderText("YYYY-MM-DD");
    m_statusEdit->setPlaceholderText("Waitlisted / Pending / Accepted / Ready / Needs Docs");
    m_statusEdit->setText("Waitlisted");
    m_roomEdit->setPlaceholderText("Target room for admit action");
    m_payerEdit->setPlaceholderText("Payer");
    m_diagnosisEdit->setPlaceholderText("Primary diagnosis summary");
    m_assessmentTypeEdit->setPlaceholderText("5-day PPS / OBRA / Managed Care Review");
    m_ardDateEdit->setPlaceholderText("ARD YYYY-MM-DD");
    m_notesEdit->setPlaceholderText("Referral notes / barriers");

    formLayout->addRow("Resident:", m_nameEdit);
    formLayout->addRow("Source:", m_sourceEdit);
    formLayout->addRow("Planned date:", m_dateEdit);
    formLayout->addRow("Status:", m_statusEdit);
    formLayout->addRow("Target room:", m_roomEdit);
    formLayout->addRow("Payer:", m_payerEdit);
    formLayout->addRow("Diagnosis:", m_diagnosisEdit);
    formLayout->addRow("MDS assessment:", m_assessmentTypeEdit);
    formLayout->addRow("ARD target:", m_ardDateEdit);
    formLayout->addRow("Notes:", m_notesEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Add to Waitlist", formCard);
    m_sendToMdsButton = new QPushButton("Send Selected to MDS", formCard);
    m_readyButton = new QPushButton("Mark Ready", formCard);
    m_needsDocsButton = new QPushButton("Needs Docs", formCard);
    m_importDocButton = new QPushButton("Import Referral Document", formCard);
    m_admitButton = new QPushButton("Admit Selected", formCard);
    m_refreshButton = new QPushButton("Refresh", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_sendToMdsButton);
    buttonRow->addWidget(m_readyButton);
    buttonRow->addWidget(m_needsDocsButton);
    buttonRow->addWidget(m_importDocButton);
    buttonRow->addWidget(m_admitButton);
    buttonRow->addWidget(m_refreshButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);

    root->addWidget(formCard);

    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(10);
    m_tableWidget->setHorizontalHeaderLabels(QStringList{
        "id", "resident_name", "referral_source", "planned_date", "payer", "diagnosis_summary",
        "assessment_type", "ard_date", "room_target", "status"
    });
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setColumnHidden(0, true);
    root->addWidget(m_tableWidget, 1);

    auto* roomHeading = new QLabel("Available / tracked rooms", this);
    roomHeading->setStyleSheet("font-size: 16px; font-weight: 700;");
    root->addWidget(roomHeading);

    auto* roomTable = new QTableWidget(this);
    const QStringList roomCols{"room_number", "status", "resident_name", "notes"};
    roomTable->setColumnCount(roomCols.size());
    roomTable->setHorizontalHeaderLabels({"Room", "Status", "Resident", "Notes"});
    roomTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (const auto& roomRow : m_db->fetchTable("bed_board", roomCols)) {
        const int rr = roomTable->rowCount();
        roomTable->insertRow(rr);
        for (int c = 0; c < roomCols.size(); ++c) roomTable->setItem(rr, c, new QTableWidgetItem(roomRow.value(roomCols[c])));
    }
    root->addWidget(roomTable);

    connect(m_addButton, &QPushButton::clicked, this, &AdmissionsPage::handleAddReferral);
    connect(m_sendToMdsButton, &QPushButton::clicked, this, &AdmissionsPage::handleSendToMds);
    connect(m_readyButton, &QPushButton::clicked, this, &AdmissionsPage::handleMarkReady);
    connect(m_needsDocsButton, &QPushButton::clicked, this, &AdmissionsPage::handleNeedsDocs);
    connect(m_importDocButton, &QPushButton::clicked, this, &AdmissionsPage::handleImportReferralDocument);
    connect(m_admitButton, &QPushButton::clicked, this, &AdmissionsPage::handleAdmitSelected);
    connect(m_refreshButton, &QPushButton::clicked, this, &AdmissionsPage::refreshTable);

    refreshTable();
}

void AdmissionsPage::refreshTable() {
    m_tableWidget->setRowCount(0);
    const QStringList keys{
        "id", "resident_name", "referral_source", "planned_date", "payer", "diagnosis_summary",
        "assessment_type", "ard_date", "room_target", "status"
    };
    const auto rows = m_db->fetchTable("admissions", keys);
    for (const auto& row : rows) {
        int r = m_tableWidget->rowCount();
        m_tableWidget->insertRow(r);
        int c = 0;
        for (const auto& key : keys) {
            m_tableWidget->setItem(r, c++, new QTableWidgetItem(row.value(key)));
        }
    }
}

void AdmissionsPage::handleAddReferral() {
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::information(this, "Add referral", "Enter a resident name first.");
        return;
    }

    const bool ok = m_db->addRecord(
        "admissions",
        {
            {"resident_name", m_nameEdit->text().trimmed()},
            {"referral_source", m_sourceEdit->text().trimmed()},
            {"planned_date", m_dateEdit->text().trimmed()},
            {"payer", m_payerEdit->text().trimmed()},
            {"diagnosis_summary", m_diagnosisEdit->text().trimmed()},
            {"assessment_type", m_assessmentTypeEdit->text().trimmed().isEmpty() ? QString("5-day PPS") : m_assessmentTypeEdit->text().trimmed()},
            {"ard_date", m_ardDateEdit->text().trimmed()},
            {"room_target", m_roomEdit->text().trimmed()},
            {"status", m_statusEdit->text().trimmed().isEmpty() ? QString("Waitlisted") : m_statusEdit->text().trimmed()},
            {"notes", m_notesEdit->text().trimmed()}
        });

    if (!ok) {
        QMessageBox::warning(this, "Add referral", "Unable to save the new waitlist referral.");
        return;
    }

    refreshTable();
}

void AdmissionsPage::handleSendToMds() {
    const int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Send to MDS", "Select a waitlist row first.");
        return;
    }

    const QString residentName = m_tableWidget->item(row, 1)->text();
    const QString payer = m_tableWidget->item(row, 4)->text();
    const QString diagnosis = m_tableWidget->item(row, 5)->text();
    const QString assessmentType = m_tableWidget->item(row, 6)->text().isEmpty() ? QString("5-day PPS") : m_tableWidget->item(row, 6)->text();
    const QString ardDate = m_tableWidget->item(row, 7)->text().isEmpty() ? QString("2026-04-26") : m_tableWidget->item(row, 7)->text();

    const bool diagnosisOk = m_db->addRecord(
        "diagnosis_items",
        {
            {"resident_name", residentName},
            {"diagnosis_summary", diagnosis},
            {"source_name", "Referral waitlist"},
            {"status", "Imported"},
            {"notes", "Imported from admissions waitlist before admit."}
        });

    const bool mdsOk = m_db->addRecord(
        "mds_items",
        {
            {"resident_name", residentName},
            {"payer", payer},
            {"assessment_type", assessmentType},
            {"ard_date", ardDate},
            {"triple_check_date", ardDate},
            {"status", "Open"},
            {"owner", "MDS Coordinator"},
            {"notes", QString("Referral intake diagnosis: %1").arg(diagnosis)}
        });

    if (!diagnosisOk || !mdsOk) {
        QMessageBox::warning(this, "Send to MDS", "Unable to move referral details into the diagnosis / MDS workflow.");
        return;
    }

    QMessageBox::information(this, "Send to MDS", "Referral details were pushed into diagnosis tracking and the MDS queue.");
}

void AdmissionsPage::handleMarkReady() {
    const int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Mark ready", "Select a waitlist row first.");
        return;
    }
    const int admissionId = m_tableWidget->item(row, 0)->text().toInt();
    if (!m_db->updateRecordById("admissions", admissionId, {{"status", "Ready"}})) {
        QMessageBox::warning(this, "Mark ready", "Unable to mark the selected referral ready.");
        return;
    }
    refreshTable();
}

void AdmissionsPage::handleNeedsDocs() {
    const int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Needs docs", "Select a waitlist row first.");
        return;
    }
    const int admissionId = m_tableWidget->item(row, 0)->text().toInt();
    if (!m_db->updateRecordById("admissions", admissionId, {{"status", "Needs Docs"}})) {
        QMessageBox::warning(this, "Needs docs", "Unable to update the selected referral status.");
        return;
    }
    refreshTable();
}

void AdmissionsPage::handleImportReferralDocument() {
    const int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Import referral document", "Select a waitlist row first.");
        return;
    }

    const QString residentName = m_tableWidget->item(row, 1)->text();
    const QString chosen = QFileDialog::getOpenFileName(this, "Select referral document to import");
    if (chosen.isEmpty()) {
        return;
    }

    const QString storageRoot = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("documents");
    QDir().mkpath(storageRoot);
    const QFileInfo info(chosen);
    const QString stampedName = QString("%1_%2").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"), info.fileName());
    const QString targetPath = QDir(storageRoot).filePath(stampedName);
    if (QFile::exists(targetPath)) {
        QFile::remove(targetPath);
    }
    if (!QFile::copy(chosen, targetPath)) {
        QMessageBox::warning(this, "Import referral document", "Unable to copy the selected file into the local document store.");
        return;
    }

    const bool ok = m_db->addRecord("document_items", {
        {"module_name", "Admissions"},
        {"document_name", info.fileName()},
        {"document_type", "Referral Intake"},
        {"linked_item", residentName},
        {"owner", "Admissions Director"},
        {"status", "Open"},
        {"file_path", targetPath},
        {"imported_on", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm")},
        {"notes", "Imported from the admissions waitlist workflow."}
    });

    if (!ok) {
        QMessageBox::warning(this, "Import referral document", "Unable to register the imported referral document.");
        return;
    }

    QMessageBox::information(this, "Import referral document", "Referral document imported and linked to the selected waitlist item.");
}

void AdmissionsPage::handleAdmitSelected() {
    const int row = m_tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Admit resident", "Select a waitlist row first.");
        return;
    }

    const int admissionId = m_tableWidget->item(row, 0)->text().toInt();
    const QString residentName = m_tableWidget->item(row, 1)->text();
    const QString payer = m_tableWidget->item(row, 4)->text();
    const QString diagnosis = m_tableWidget->item(row, 5)->text();
    const QString assessmentType = m_tableWidget->item(row, 6)->text().isEmpty() ? QString("5-day PPS") : m_tableWidget->item(row, 6)->text();
    const QString ardDate = m_tableWidget->item(row, 7)->text().isEmpty() ? QString("2026-04-26") : m_tableWidget->item(row, 7)->text();
    const QString room = m_tableWidget->item(row, 8)->text().isEmpty() ? QString("TBD") : m_tableWidget->item(row, 8)->text();
    const QString status = m_tableWidget->item(row, 9)->text();

    if (status.compare("Admitted", Qt::CaseInsensitive) == 0) {
        QMessageBox::information(this, "Admit resident", "That waitlist item is already marked admitted.");
        return;
    }

    if (!m_db->admitResident(residentName, room, payer, admissionId, diagnosis)) {
        QMessageBox::warning(this, "Admit resident", "Unable to admit the selected resident.");
        return;
    }

    m_db->addRecord(
        "diagnosis_items",
        {
            {"resident_name", residentName},
            {"diagnosis_summary", diagnosis},
            {"source_name", "Admission"},
            {"status", "Active"},
            {"notes", "Created automatically during waitlist admit."}
        });

    m_db->addRecord(
        "mds_items",
        {
            {"resident_name", residentName},
            {"payer", payer},
            {"assessment_type", assessmentType},
            {"ard_date", ardDate},
            {"triple_check_date", ardDate},
            {"status", "Open"},
            {"owner", "MDS Coordinator"},
            {"notes", QString("Created automatically from admitted waitlist referral. Diagnosis: %1").arg(diagnosis)}
        });

    refreshTable();
}
