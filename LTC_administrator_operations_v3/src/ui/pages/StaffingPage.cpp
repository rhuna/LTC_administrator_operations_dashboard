
#include "StaffingPage.h"
#include "../../data/DatabaseManager.h"

#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {
QString ratioText(int residents, int staff) {
    if (staff <= 0) return "Open gap";
    return QString("%1 : 1").arg(QString::number(static_cast<double>(residents) / static_cast<double>(staff), 'f', 1));
}
}

StaffingPage::StaffingPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("HR / Staffing Live Numbers & Ratios", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel("Update current house staffing numbers, see ratios immediately, and identify where staffing is lacking against minimum coverage.", this);
    subheading->setWordWrap(true);
    m_snapshotLabel = new QLabel(this);
    m_snapshotLabel->setStyleSheet("font-weight: 600; color: #334155;");
    root->addWidget(heading);
    root->addWidget(subheading);
    root->addWidget(m_snapshotLabel);

    auto* formCard = new QGroupBox("Live staffing update", this);
    auto* formLayout = new QFormLayout(formCard);
    m_dateEdit = new QLineEdit(formCard);
    m_shiftEdit = new QLineEdit(formCard);
    m_censusEdit = new QLineEdit(formCard);
    m_rnEdit = new QLineEdit(formCard);
    m_lpnEdit = new QLineEdit(formCard);
    m_cnaEdit = new QLineEdit(formCard);
    m_agencyEdit = new QLineEdit(formCard);
    m_notesEdit = new QLineEdit(formCard);
    m_dateEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_shiftEdit->setPlaceholderText("Day / Evening / Night");
    m_censusEdit->setPlaceholderText("Current resident census");
    m_rnEdit->setPlaceholderText("RN count");
    m_lpnEdit->setPlaceholderText("LPN count");
    m_cnaEdit->setPlaceholderText("CNA count");
    m_agencyEdit->setPlaceholderText("Agency count");
    m_notesEdit->setPlaceholderText("What is lacking / notes");
    formLayout->addRow("Date:", m_dateEdit);
    formLayout->addRow("Shift:", m_shiftEdit);
    formLayout->addRow("Resident census:", m_censusEdit);
    formLayout->addRow("RN:", m_rnEdit);
    formLayout->addRow("LPN:", m_lpnEdit);
    formLayout->addRow("CNA:", m_cnaEdit);
    formLayout->addRow("Agency:", m_agencyEdit);
    formLayout->addRow("Notes:", m_notesEdit);
    auto* buttons = new QHBoxLayout();
    m_addButton = new QPushButton("Update Live Numbers", formCard);
    m_refreshButton = new QPushButton("Refresh Ratios", formCard);
    buttons->addWidget(m_addButton);
    buttons->addWidget(m_refreshButton);
    buttons->addStretch();
    formLayout->addRow(buttons);
    root->addWidget(formCard);

    auto* ratioCard = new QGroupBox("Live ratio view", this);
    auto* ratioLayout = new QVBoxLayout(ratioCard);
    m_ratioTable = new QTableWidget(ratioCard);
    m_ratioTable->setColumnCount(7);
    m_ratioTable->setHorizontalHeaderLabels(QStringList{"entry_date", "shift_name", "resident_census", "residents_per_cna", "residents_per_licensed", "residents_per_total_staff", "agency_share"});
    m_ratioTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ratioLayout->addWidget(m_ratioTable);
    root->addWidget(ratioCard);

    auto* minimumCard = new QGroupBox("Minimum staffing gap view", this);
    auto* minimumLayout = new QVBoxLayout(minimumCard);
    m_minimumTable = new QTableWidget(minimumCard);
    m_minimumTable->setColumnCount(5);
    m_minimumTable->setHorizontalHeaderLabels(QStringList{"shift_name", "required_nursing", "entered_nursing", "gap_count", "status"});
    m_minimumTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    minimumLayout->addWidget(m_minimumTable);
    root->addWidget(minimumCard);

    QObject::connect(m_addButton, &QPushButton::clicked, this, &StaffingPage::handleAddNumbers);
    QObject::connect(m_refreshButton, &QPushButton::clicked, this, &StaffingPage::refreshTables);

    refreshTables();
}

void StaffingPage::handleAddNumbers() {
    if (!m_db) {
        return;
    }
    m_db->addStaffingNumbersEntry(
        m_dateEdit->text(),
        m_shiftEdit->text(),
        m_censusEdit->text().toInt(),
        m_rnEdit->text().toInt(),
        m_lpnEdit->text().toInt(),
        m_cnaEdit->text().toInt(),
        m_agencyEdit->text().toInt(),
        m_notesEdit->text());
    refreshTables();
}

void StaffingPage::refreshTables() {
    if (!m_db) {
        return;
    }

    const auto rows = m_db->fetchTable("staffing_number_entries", {"entry_date","shift_name","resident_census","rn_count","lpn_count","cna_count","agency_count","notes","id"});
    m_ratioTable->setRowCount(0);

    if (!rows.isEmpty()) {
        const auto latest = rows.last();
        const int census = latest.value("resident_census").toInt();
        const int rn = latest.value("rn_count").toInt();
        const int lpn = latest.value("lpn_count").toInt();
        const int cna = latest.value("cna_count").toInt();
        const int agency = latest.value("agency_count").toInt();
        const int licensed = rn + lpn;
        const int total = rn + lpn + cna + agency;
        m_snapshotLabel->setText(
            QString("Latest %1 shift: census %2 · residents/CNA %3 · residents/licensed %4 · residents/total staff %5")
                .arg(latest.value("shift_name"))
                .arg(census)
                .arg(ratioText(census, cna))
                .arg(ratioText(census, licensed))
                .arg(ratioText(census, total)));
    } else {
        m_snapshotLabel->setText("No staffing numbers entered yet.");
    }

    for (const auto& row : rows) {
        const int r = m_ratioTable->rowCount();
        m_ratioTable->insertRow(r);
        const int census = row.value("resident_census").toInt();
        const int rn = row.value("rn_count").toInt();
        const int lpn = row.value("lpn_count").toInt();
        const int cna = row.value("cna_count").toInt();
        const int agency = row.value("agency_count").toInt();
        const int licensed = rn + lpn;
        const int total = rn + lpn + cna + agency;
        m_ratioTable->setItem(r, 0, new QTableWidgetItem(row.value("entry_date")));
        m_ratioTable->setItem(r, 1, new QTableWidgetItem(row.value("shift_name")));
        m_ratioTable->setItem(r, 2, new QTableWidgetItem(QString::number(census)));
        m_ratioTable->setItem(r, 3, new QTableWidgetItem(ratioText(census, cna)));
        m_ratioTable->setItem(r, 4, new QTableWidgetItem(ratioText(census, licensed)));
        m_ratioTable->setItem(r, 5, new QTableWidgetItem(ratioText(census, total)));
        m_ratioTable->setItem(r, 6, new QTableWidgetItem(total > 0 ? QString("%1%").arg(QString::number((agency * 100.0) / total, 'f', 0)) : "0%"));
    }

    const auto minimumRows = m_db->staffingMinimumSummary();
    m_minimumTable->setRowCount(0);
    for (const auto& row : minimumRows) {
        const int r = m_minimumTable->rowCount();
        m_minimumTable->insertRow(r);
        m_minimumTable->setItem(r, 0, new QTableWidgetItem(row.value("shift_name")));
        m_minimumTable->setItem(r, 1, new QTableWidgetItem(row.value("required_nursing")));
        m_minimumTable->setItem(r, 2, new QTableWidgetItem(row.value("entered_nursing")));
        m_minimumTable->setItem(r, 3, new QTableWidgetItem(row.value("gap_count")));
        m_minimumTable->setItem(r, 4, new QTableWidgetItem(row.value("status")));
    }
}
