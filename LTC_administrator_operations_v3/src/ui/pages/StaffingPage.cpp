
#include "StaffingPage.h"
#include "../../data/DatabaseManager.h"

#include <QDate>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {
QLabel* makeStatValue(const QString& value, QWidget* parent) {
    auto* label = new QLabel(value, parent);
    label->setStyleSheet("font-size: 26px; font-weight: 700; color: #0f172a;");
    return label;
}

QLabel* makeStatCaption(const QString& text, QWidget* parent) {
    auto* label = new QLabel(text, parent);
    label->setWordWrap(true);
    label->setStyleSheet("font-size: 11px; color: #64748b;");
    return label;
}

QString ratioText(int residents, int staff) {
    if (staff <= 0) return "—";
    return QString("%1 : 1").arg(QString::number(static_cast<double>(residents) / static_cast<double>(staff), 'f', 1));
}

int toInt(const QMap<QString, QString>& row, const QString& key) {
    return row.value(key).toInt();
}
}

StaffingPage::StaffingPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(14);

    auto* heading = new QLabel("Staffing Numbers & Ratios", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    auto* subheading = new QLabel(
        "Enter house staffing numbers by shift, review resident-to-staff ratios, and compare the entered staffing counts to your configured minimum staffing targets. This keeps staffing focused on the numbers, not hiring or firing workflow.",
        this);
    subheading->setWordWrap(true);
    subheading->setStyleSheet("color: #5b6472;");
    m_snapshotLabel = new QLabel(this);
    m_snapshotLabel->setStyleSheet("font-weight: 600; color: #334155;");
    root->addWidget(heading);
    root->addWidget(subheading);
    root->addWidget(m_snapshotLabel);

    auto* statRow = new QHBoxLayout();
    statRow->setSpacing(10);
    auto addStatCard = [&](const QString& caption, QLabel** outValue) {
        auto* card = new QFrame(this);
        card->setStyleSheet("QFrame { background: #f8fafc; border: 1px solid #e2e8f0; border-radius: 14px; }");
        auto* layout = new QVBoxLayout(card);
        layout->setContentsMargins(14, 12, 14, 12);
        layout->setSpacing(4);
        *outValue = makeStatValue("0", card);
        layout->addWidget(*outValue);
        layout->addWidget(makeStatCaption(caption, card));
        statRow->addWidget(card, 1);
    };
    addStatCard("Latest resident census", &m_latestCensusLabel);
    addStatCard("Latest total nursing staff", &m_totalNursingLabel);
    addStatCard("Residents per CNA", &m_cnaRatioLabel);
    addStatCard("Residents per licensed nurse", &m_licensedRatioLabel);
    addStatCard("Residents per total staff", &m_totalRatioLabel);
    root->addLayout(statRow);

    auto* formCard = new QGroupBox("Enter staffing numbers", this);
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
    m_notesEdit->setPlaceholderText("Optional note");

    formLayout->addRow("Entry date:", m_dateEdit);
    formLayout->addRow("Shift:", m_shiftEdit);
    formLayout->addRow("Resident census:", m_censusEdit);
    formLayout->addRow("RN count:", m_rnEdit);
    formLayout->addRow("LPN count:", m_lpnEdit);
    formLayout->addRow("CNA count:", m_cnaEdit);
    formLayout->addRow("Agency count:", m_agencyEdit);
    formLayout->addRow("Notes:", m_notesEdit);

    auto* buttonRow = new QHBoxLayout();
    m_addButton = new QPushButton("Save Staffing Numbers", formCard);
    m_refreshButton = new QPushButton("Refresh", formCard);
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_refreshButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formCard);

    auto* entriesCard = new QGroupBox("Saved staffing entries", this);
    auto* entriesLayout = new QVBoxLayout(entriesCard);
    m_entriesTable = new QTableWidget(entriesCard);
    m_entriesTable->setColumnCount(11);
    m_entriesTable->setHorizontalHeaderLabels(QStringList{"entry_date", "shift_name", "resident_census", "rn_count", "lpn_count", "cna_count", "agency_count", "licensed_total", "nursing_total", "total_staff", "notes"});
    m_entriesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_entriesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    entriesLayout->addWidget(m_entriesTable);
    root->addWidget(entriesCard, 1);

    auto* bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(14);

    auto* ratioCard = new QGroupBox("Ratio view by entry", this);
    auto* ratioLayout = new QVBoxLayout(ratioCard);
    auto* ratioHint = new QLabel("Quick ratio view using each saved staffing entry so you can compare CNA, licensed, and total-staff coverage against the resident census.", ratioCard);
    ratioHint->setWordWrap(true);
    ratioHint->setStyleSheet("color: #5b6472;");
    m_ratioTable = new QTableWidget(ratioCard);
    m_ratioTable->setColumnCount(6);
    m_ratioTable->setHorizontalHeaderLabels(QStringList{"entry_date", "shift_name", "residents_per_cna", "residents_per_licensed", "residents_per_total_staff", "agency_share"});
    m_ratioTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ratioTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ratioLayout->addWidget(ratioHint);
    ratioLayout->addWidget(m_ratioTable);
    bottomRow->addWidget(ratioCard, 3);

    auto* minimumCard = new QGroupBox("Minimum staffing comparison", this);
    auto* minimumLayout = new QVBoxLayout(minimumCard);
    auto* minimumHint = new QLabel("This compares the latest entered total nursing count for each shift to the configured minimum nursing requirement for that shift.", minimumCard);
    minimumHint->setWordWrap(true);
    minimumHint->setStyleSheet("color: #5b6472;");
    m_minimumTable = new QTableWidget(minimumCard);
    m_minimumTable->setColumnCount(5);
    m_minimumTable->setHorizontalHeaderLabels(QStringList{"shift_name", "required_nursing", "entered_nursing", "gap_count", "status"});
    m_minimumTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_minimumTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    minimumLayout->addWidget(minimumHint);
    minimumLayout->addWidget(m_minimumTable);
    bottomRow->addWidget(minimumCard, 2);

    root->addLayout(bottomRow, 1);

    connect(m_addButton, &QPushButton::clicked, this, &StaffingPage::handleAddNumbers);
    connect(m_refreshButton, &QPushButton::clicked, this, &StaffingPage::refreshTables);

    refreshTables();
}

void StaffingPage::handleAddNumbers() {
    const bool ok = m_db->addStaffingNumbersEntry(
        m_dateEdit->text().trimmed(),
        m_shiftEdit->text().trimmed(),
        m_censusEdit->text().trimmed().toInt(),
        m_rnEdit->text().trimmed().toInt(),
        m_lpnEdit->text().trimmed().toInt(),
        m_cnaEdit->text().trimmed().toInt(),
        m_agencyEdit->text().trimmed().toInt(),
        m_notesEdit->text().trimmed());

    if (ok) {
        m_shiftEdit->clear();
        m_censusEdit->clear();
        m_rnEdit->clear();
        m_lpnEdit->clear();
        m_cnaEdit->clear();
        m_agencyEdit->clear();
        m_notesEdit->clear();
        refreshTables();
    }
}

void StaffingPage::refreshTables() {
    const QStringList cols{"entry_date", "shift_name", "resident_census", "rn_count", "lpn_count", "cna_count", "agency_count", "notes"};
    const auto rows = m_db->fetchTable("staffing_number_entries", cols);

    m_entriesTable->setRowCount(0);
    m_ratioTable->setRowCount(0);

    int latestCensus = 0;
    int latestRn = 0;
    int latestLpn = 0;
    int latestCna = 0;
    int latestAgency = 0;
    QString latestShift;

    QMap<QString, QMap<QString, int>> latestByShift;

    for (const auto& row : rows) {
        const int census = toInt(row, "resident_census");
        const int rn = toInt(row, "rn_count");
        const int lpn = toInt(row, "lpn_count");
        const int cna = toInt(row, "cna_count");
        const int agency = toInt(row, "agency_count");
        const int licensed = rn + lpn;
        const int nursing = licensed + cna;
        const int totalStaff = nursing + agency;

        int r = m_entriesTable->rowCount();
        m_entriesTable->insertRow(r);
        const QStringList entryVals{
            row.value("entry_date"), row.value("shift_name"), QString::number(census), QString::number(rn),
            QString::number(lpn), QString::number(cna), QString::number(agency), QString::number(licensed),
            QString::number(nursing), QString::number(totalStaff), row.value("notes")
        };
        for (int c = 0; c < entryVals.size(); ++c) m_entriesTable->setItem(r, c, new QTableWidgetItem(entryVals[c]));

        r = m_ratioTable->rowCount();
        m_ratioTable->insertRow(r);
        const QString agencyShare = totalStaff > 0 ? QString("%1%").arg(QString::number((agency * 100.0) / totalStaff, 'f', 1)) : "0.0%";
        const QStringList ratioVals{
            row.value("entry_date"),
            row.value("shift_name"),
            ratioText(census, cna),
            ratioText(census, licensed),
            ratioText(census, totalStaff),
            agencyShare
        };
        for (int c = 0; c < ratioVals.size(); ++c) m_ratioTable->setItem(r, c, new QTableWidgetItem(ratioVals[c]));

        latestCensus = census;
        latestRn = rn;
        latestLpn = lpn;
        latestCna = cna;
        latestAgency = agency;
        latestShift = row.value("shift_name");

        latestByShift[row.value("shift_name")]["nursing"] = nursing;
    }

    const int latestLicensed = latestRn + latestLpn;
    const int latestNursing = latestLicensed + latestCna;
    const int latestTotal = latestNursing + latestAgency;

    m_latestCensusLabel->setText(QString::number(latestCensus));
    m_totalNursingLabel->setText(QString::number(latestNursing));
    m_cnaRatioLabel->setText(ratioText(latestCensus, latestCna));
    m_licensedRatioLabel->setText(ratioText(latestCensus, latestLicensed));
    m_totalRatioLabel->setText(ratioText(latestCensus, latestTotal));

    m_snapshotLabel->setText(
        QString("Latest staffing entry: %1 shift · %2 census · %3 total nursing · %4 agency. Enter your house numbers here and use the ratios below for quick staffing review.")
            .arg(latestShift.isEmpty() ? "No" : latestShift)
            .arg(latestCensus)
            .arg(latestNursing)
            .arg(latestAgency));

    // minimum comparison uses configured nursing minimums aggregated by shift
    const auto minimumRows = m_db->fetchTable("staffing_minimums", {"shift_name", "department", "minimum_required"});
    QMap<QString, int> requiredByShift;
    for (const auto& row : minimumRows) {
        if (row.value("department").compare("Nursing", Qt::CaseInsensitive) == 0) {
            requiredByShift[row.value("shift_name")] += row.value("minimum_required").toInt();
        }
    }

    m_minimumTable->setRowCount(0);
    for (auto it = requiredByShift.constBegin(); it != requiredByShift.constEnd(); ++it) {
        const QString shift = it.key();
        const int required = it.value();
        const int entered = latestByShift.value(shift).value("nursing");
        const int gap = qMax(0, required - entered);
        const QString status = gap > 0 ? "Below minimum" : "At / above minimum";
        const int r = m_minimumTable->rowCount();
        m_minimumTable->insertRow(r);
        const QStringList vals{shift, QString::number(required), QString::number(entered), QString::number(gap), status};
        for (int c = 0; c < vals.size(); ++c) m_minimumTable->setItem(r, c, new QTableWidgetItem(vals[c]));
    }
}
