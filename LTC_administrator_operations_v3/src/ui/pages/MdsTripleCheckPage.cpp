#include "MdsTripleCheckPage.h"
#include "../../data/DatabaseManager.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

MdsTripleCheckPage::MdsTripleCheckPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("MDS / ARD / Triple Check / Therapy", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Track MDS due dates, ARD targets, triple-check readiness, diagnosis detail, therapy / rehab items, and managed-care reimbursement follow-up from one combined MDS workspace.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* mdsTable = new QTableWidget(this);
    const QStringList mdsCols{
        "resident_name", "payer", "assessment_type", "ard_date",
        "triple_check_date", "status", "owner", "notes"
    };
    mdsTable->setColumnCount(mdsCols.size());
    mdsTable->setHorizontalHeaderLabels(
        {"Resident", "Payer", "Assessment", "ARD", "Triple Check", "Status", "Owner", "Notes"});
    mdsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshMdsTable = [db, mdsTable, mdsCols]() {
        mdsTable->setRowCount(0);
        const auto rows = db->fetchTable("mds_items", mdsCols);
        for (const auto& row : rows) {
            const int r = mdsTable->rowCount();
            mdsTable->insertRow(r);
            for (int c = 0; c < mdsCols.size(); ++c) {
                mdsTable->setItem(r, c, new QTableWidgetItem(row.value(mdsCols[c])));
            }
        }
    };

    auto* form = new QHBoxLayout();
    auto* residentName = new QLineEdit(this);
    residentName->setPlaceholderText("Resident");
    auto* payer = new QLineEdit(this);
    payer->setPlaceholderText("Payer");
    auto* assessmentType = new QLineEdit(this);
    assessmentType->setPlaceholderText("5-day / PPS / OBRA");
    auto* ardDate = new QLineEdit(this);
    ardDate->setPlaceholderText("ARD YYYY-MM-DD");
    auto* tripleCheckDate = new QLineEdit(this);
    tripleCheckDate->setPlaceholderText("Triple-check YYYY-MM-DD");
    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Owner");
    auto* notes = new QLineEdit(this);
    notes->setPlaceholderText("Notes");
    auto* button = new QPushButton("Add MDS Item", this);

    form->addWidget(residentName);
    form->addWidget(payer);
    form->addWidget(assessmentType);
    form->addWidget(ardDate);
    form->addWidget(tripleCheckDate);
    form->addWidget(owner);
    form->addWidget(notes);
    form->addWidget(button);

    root->addLayout(form);
    root->addWidget(mdsTable);

    auto* therapyHeading = new QLabel("Therapy / rehab items", this);
    therapyHeading->setStyleSheet("font-size: 16px; font-weight: 700;");
    root->addWidget(therapyHeading);

    auto* therapyTable = new QTableWidget(this);
    const QStringList therapyCols{"review_date", "resident_name", "discipline", "item_name", "owner", "status", "notes"};
    therapyTable->setColumnCount(therapyCols.size());
    therapyTable->setHorizontalHeaderLabels({"Date", "Resident", "Discipline", "Item", "Owner", "Status", "Notes"});
    therapyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshTherapyTable = [db, therapyTable, therapyCols]() {
        therapyTable->setRowCount(0);
        const auto rows = db->fetchTable("therapy_items", therapyCols);
        for (const auto& row : rows) {
            const int r = therapyTable->rowCount();
            therapyTable->insertRow(r);
            for (int c = 0; c < therapyCols.size(); ++c) {
                therapyTable->setItem(r, c, new QTableWidgetItem(row.value(therapyCols[c])));
            }
        }
    };
    root->addWidget(therapyTable);

    auto* diagnosisHeading = new QLabel("Imported diagnosis detail", this);
    diagnosisHeading->setStyleSheet("font-size: 16px; font-weight: 700;");
    root->addWidget(diagnosisHeading);

    auto* diagnosisTable = new QTableWidget(this);
    const QStringList diagnosisCols{"resident_name", "diagnosis_summary", "source_name", "status", "notes"};
    diagnosisTable->setColumnCount(diagnosisCols.size());
    diagnosisTable->setHorizontalHeaderLabels({"Resident", "Diagnosis Summary", "Source", "Status", "Notes"});
    diagnosisTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshDiagnosisTable = [db, diagnosisTable, diagnosisCols]() {
        diagnosisTable->setRowCount(0);
        const auto rows = db->fetchTable("diagnosis_items", diagnosisCols);
        for (const auto& row : rows) {
            const int r = diagnosisTable->rowCount();
            diagnosisTable->insertRow(r);
            for (int c = 0; c < diagnosisCols.size(); ++c) {
                diagnosisTable->setItem(r, c, new QTableWidgetItem(row.value(diagnosisCols[c])));
            }
        }
    };
    root->addWidget(diagnosisTable);

    auto* managedHeading = new QLabel("Managed care / reimbursement follow-up", this);
    managedHeading->setStyleSheet("font-size: 16px; font-weight: 700;");
    root->addWidget(managedHeading);

    auto* managedTable = new QTableWidget(this);
    const QStringList managedCols{"resident_name", "payer", "item_name", "auth_expiry", "owner", "status"};
    managedTable->setColumnCount(managedCols.size());
    managedTable->setHorizontalHeaderLabels(
        {"Resident", "Payer", "Item / Auth", "Auth Expiry", "Owner", "Status"});
    managedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    const auto managedRows = db->fetchTable("managed_care_items", managedCols);
    for (const auto& row : managedRows) {
        const int r = managedTable->rowCount();
        managedTable->insertRow(r);
        for (int c = 0; c < managedCols.size(); ++c) {
            managedTable->setItem(r, c, new QTableWidgetItem(row.value(managedCols[c])));
        }
    }
    root->addWidget(managedTable);

    QObject::connect(button, &QPushButton::clicked, this, [=]() {
        QMap<QString, QString> values{
            {"resident_name", residentName->text()},
            {"payer", payer->text()},
            {"assessment_type", assessmentType->text()},
            {"ard_date", ardDate->text().isEmpty() ? "2026-04-23" : ardDate->text()},
            {"triple_check_date", tripleCheckDate->text().isEmpty() ? "2026-04-24" : tripleCheckDate->text()},
            {"status", "Open"},
            {"owner", owner->text().isEmpty() ? "MDS Coordinator" : owner->text()},
            {"notes", notes->text()}
        };
        db->addRecord("mds_items", values);
        refreshMdsTable();
        residentName->clear();
        payer->clear();
        assessmentType->clear();
        ardDate->clear();
        tripleCheckDate->clear();
        owner->clear();
        notes->clear();
    });

    refreshMdsTable();
    refreshTherapyTable();
    refreshDiagnosisTable();
}
