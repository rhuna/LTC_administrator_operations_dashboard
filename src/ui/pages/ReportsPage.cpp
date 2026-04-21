#include "ReportsPage.h"
#include "../../data/DatabaseManager.h"

#include <QDate>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPushButton>
#include <QSaveFile>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

ReportsPage::ReportsPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Reports & Export", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Create a leadership-ready daily summary, export census and staffing snapshots, and print a concise executive report without digging through every module.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* topBox = new QGroupBox("Quick report actions", this);
    auto* topLayout = new QGridLayout(topBox);

    m_refreshButton = new QPushButton("Refresh Preview", this);
    m_exportDailyButton = new QPushButton("Export Daily Summary", this);
    m_exportCensusButton = new QPushButton("Export Census CSV", this);
    m_exportStaffingButton = new QPushButton("Export Staffing CSV", this);
    m_printButton = new QPushButton("Print Daily Summary", this);

    auto* helper = new QLabel(
        "Use the preview below to sanity-check the report, then export or print it for shift huddle, leadership review, or survey preparation.",
        this);
    helper->setWordWrap(true);

    topLayout->addWidget(m_refreshButton, 0, 0);
    topLayout->addWidget(m_exportDailyButton, 0, 1);
    topLayout->addWidget(m_exportCensusButton, 0, 2);
    topLayout->addWidget(m_exportStaffingButton, 1, 0);
    topLayout->addWidget(m_printButton, 1, 1);
    topLayout->addWidget(helper, 2, 0, 1, 3);

    root->addWidget(topBox);

    auto* previewBox = new QGroupBox("Daily executive summary preview", this);
    auto* previewLayout = new QVBoxLayout(previewBox);
    m_preview = new QTextEdit(this);
    m_preview->setReadOnly(true);
    m_preview->setMinimumHeight(540);
    previewLayout->addWidget(m_preview);
    root->addWidget(previewBox, 1);

    connect(m_refreshButton, &QPushButton::clicked, this, &ReportsPage::refreshPreview);
    connect(m_exportDailyButton, &QPushButton::clicked, this, &ReportsPage::exportDailySummary);
    connect(m_exportCensusButton, &QPushButton::clicked, this, &ReportsPage::exportCensusCsv);
    connect(m_exportStaffingButton, &QPushButton::clicked, this, &ReportsPage::exportStaffingCsv);
    connect(m_printButton, &QPushButton::clicked, this, &ReportsPage::printDailySummary);

    refreshPreview();
}

QString ReportsPage::buildDailySummaryText() const {
    QString text;
    QTextStream stream(&text);

    stream << "LTC Administrator Operations Dashboard v25\n";
    stream << "Daily Executive Summary\n";
    stream << "Generated: " << QDate::currentDate().toString("yyyy-MM-dd") << "\n\n";

    stream << "Census and flow\n";
    stream << "- Current residents: " << m_db->countWhere("residents", "status='Current'") << "\n";
    stream << "- Pending admissions: " << m_db->countWhere("admissions", "status='Pending' OR status='Accepted'") << "\n";
    stream << "- Discharged residents: " << m_db->countWhere("residents", "status='Discharged'") << "\n";
    stream << "- Open bed board / turnover items: " << m_db->countWhere("bed_board", "status!='Closed'") << "\n\n";

    stream << "Staffing snapshot\n";
    stream << "- Open staffing assignments: " << m_db->countWhere("staffing_assignments", "status='Open'") << "\n";
    stream << "- Filled staffing assignments: " << m_db->countWhere("staffing_assignments", "status='Filled'") << "\n";
    stream << "- Minimum staffing gaps: " << m_db->countMinimumStaffingGaps() << "\n";
    stream << "- Estimated uncovered minimum hours: " << m_db->estimatedMinimumHoursGap() << "\n";
    stream << "- Estimated nursing HPRD: " << QString::number(m_db->estimatedNursingHprd(), 'f', 2) << "\n\n";

    stream << "Operational risk\n";
    stream << "- Open tasks: " << m_db->countWhere("tasks", "status!='Complete'") << "\n";
    stream << "- Open incidents: " << m_db->countWhere("incidents", "status!='Closed'") << "\n";
    stream << "- Open survey-readiness items: " << m_db->countWhere("survey_items", "status!='Closed'") << "\n";
    stream << "- Managed-care items at risk/open: " << m_db->countWhere("managed_care_items", "status='At Risk' OR status='Open'") << "\n";
    stream << "- Transport items needing follow-up: " << m_db->countWhere("transport_items", "status!='Returned' AND status!='Closed'") << "\n";
    stream << "- Pharmacy items open/watch: " << m_db->countWhere("pharmacy_items", "status='Open' OR status='Watch' OR status='In Progress'") << "\n";
    stream << "- Dietary items open/watch: " << m_db->countWhere("dietary_items", "status='Open' OR status='Watch' OR status='In Progress'") << "\n\n";

    stream << "Action center highlights\n";
    const auto items = m_db->actionCenterItems();
    for (const auto& item : items) {
        stream << "- " << item.first << ": " << item.second << "\n";
    }

    return text;
}

QString ReportsPage::buildCensusCsv() const {
    const QStringList cols{"resident_name", "room", "payer", "status"};
    const auto rows = m_db->fetchTable("residents", cols);

    QString csv;
    QTextStream stream(&csv);
    stream << "Resident,Room,Payer,Status\n";
    for (const auto& row : rows) {
        stream << '"' << row.value("resident_name") << "\",";
        stream << '"' << row.value("room") << "\",";
        stream << '"' << row.value("payer") << "\",";
        stream << '"' << row.value("status") << "\"\n";
    }
    return csv;
}

QString ReportsPage::buildStaffingCsv() const {
    const QStringList cols{"work_date", "department", "shift_name", "role_name", "employee_name", "status"};
    const auto rows = m_db->fetchTable("staffing_assignments", cols);

    QString csv;
    QTextStream stream(&csv);
    stream << "Work Date,Department,Shift,Role,Employee,Coverage Status\n";
    for (const auto& row : rows) {
        stream << '"' << row.value("work_date") << "\",";
        stream << '"' << row.value("department") << "\",";
        stream << '"' << row.value("shift_name") << "\",";
        stream << '"' << row.value("role_name") << "\",";
        stream << '"' << row.value("employee_name") << "\",";
        stream << '"' << row.value("status") << "\"\n";
    }
    return csv;
}

bool ReportsPage::saveTextToFile(const QString& suggestedName, const QString& filter, const QString& content) const {
    const QString path = QFileDialog::getSaveFileName(nullptr, "Save Report", suggestedName, filter);
    if (path.isEmpty()) {
        return false;
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Save failed", "Could not open the selected file for writing.");
        return false;
    }

    QTextStream stream(&file);
    stream << content;
    if (!file.commit()) {
        QMessageBox::warning(nullptr, "Save failed", "Could not finalize the report file.");
        return false;
    }
    return true;
}

void ReportsPage::refreshPreview() {
    m_preview->setPlainText(buildDailySummaryText());
}

void ReportsPage::exportDailySummary() {
    if (saveTextToFile("ltc_daily_summary_v25.txt", "Text Files (*.txt)", buildDailySummaryText())) {
        QMessageBox::information(this, "Report exported", "The daily summary was exported successfully.");
    }
}

void ReportsPage::exportCensusCsv() {
    if (saveTextToFile("ltc_census_snapshot_v25.csv", "CSV Files (*.csv)", buildCensusCsv())) {
        QMessageBox::information(this, "Export complete", "The census CSV was exported successfully.");
    }
}

void ReportsPage::exportStaffingCsv() {
    if (saveTextToFile("ltc_staffing_snapshot_v25.csv", "CSV Files (*.csv)", buildStaffingCsv())) {
        QMessageBox::information(this, "Export complete", "The staffing CSV was exported successfully.");
    }
}

void ReportsPage::printDailySummary() {
    QPrinter printer(QPrinter::HighResolution);
    printer.setDocName("LTC Daily Executive Summary v25");
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_preview->document()->print(&printer);
    }
}
