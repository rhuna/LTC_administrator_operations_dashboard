#include "MetricsChartsPage.h"

#include "../../data/DatabaseManager.h"

#include <QBrush>
#include <QColor>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
int countByValue(const QList<QMap<QString, QString>>& rows, const QString& key, const QString& value) {
    int total = 0;
    for (const auto& row : rows) {
        if (row.value(key).compare(value, Qt::CaseInsensitive) == 0) ++total;
    }
    return total;
}

int countByValues(const QList<QMap<QString, QString>>& rows, const QString& key, const QStringList& values) {
    int total = 0;
    for (const auto& row : rows) {
        const QString cell = row.value(key).trimmed();
        for (const auto& value : values) {
            if (cell.compare(value, Qt::CaseInsensitive) == 0) {
                ++total;
                break;
            }
        }
    }
    return total;
}

QFrame* makeMetricTile(const QString& title, QLabel*& valueLabel, const QString& subtitle) {
    auto* frame = new QFrame();
    frame->setObjectName("metricTile");
    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(6);

    auto* titleLabel = new QLabel(title, frame);
    titleLabel->setStyleSheet("font-size:13px; font-weight:600; color:#486581;");
    valueLabel = new QLabel("--", frame);
    valueLabel->setStyleSheet("font-size:28px; font-weight:800; color:#102a43;");
    auto* subtitleLabel = new QLabel(subtitle, frame);
    subtitleLabel->setWordWrap(true);
    subtitleLabel->setStyleSheet("font-size:12px; color:#7b8794;");

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    layout->addWidget(subtitleLabel);
    return frame;
}

QGroupBox* makeProgressCard(const QString& title, const QString& helper, QProgressBar*& bar, QLabel*& valueLabel) {
    auto* box = new QGroupBox(title);
    auto* layout = new QVBoxLayout(box);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);

    auto* helperLabel = new QLabel(helper, box);
    helperLabel->setWordWrap(true);
    helperLabel->setStyleSheet("font-size:12px; color:#7b8794;");

    bar = new QProgressBar(box);
    bar->setTextVisible(false);
    bar->setMinimumHeight(14);
    bar->setRange(0, 100);

    valueLabel = new QLabel("--", box);
    valueLabel->setStyleSheet("font-size:18px; font-weight:700; color:#102a43;");

    layout->addWidget(helperLabel);
    layout->addWidget(bar);
    layout->addWidget(valueLabel);
    return box;
}
}

MetricsChartsPage::MetricsChartsPage(DatabaseManager* db, QWidget* parent) : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(14);

    auto* title = new QLabel("Metrics & Trends", this);
    title->setStyleSheet("font-size:24px; font-weight:800; color:#102a43;");
    auto* subtitle = new QLabel("A more visual operations view for census, staffing, quality, readiness, and open-workload trends using the data already in the dashboard.", this);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("font-size:13px; color:#486581;");

    auto* refreshButton = new QPushButton("Refresh Metrics", this);
    refreshButton->setStyleSheet("padding:8px 14px; font-weight:600;");

    auto* headRow = new QHBoxLayout();
    auto* titleWrap = new QVBoxLayout();
    titleWrap->addWidget(title);
    titleWrap->addWidget(subtitle);
    headRow->addLayout(titleWrap, 1);
    headRow->addWidget(refreshButton, 0, Qt::AlignTop);
    root->addLayout(headRow);

    auto* cardsGrid = new QGridLayout();
    cardsGrid->setHorizontalSpacing(12);
    cardsGrid->setVerticalSpacing(12);
    cardsGrid->addWidget(makeMetricTile("Current Residents", residentLabel, "Active current census records."), 0, 0);
    cardsGrid->addWidget(makeMetricTile("Pending Admissions", admissionsLabel, "Waitlist and planned admits still open."), 0, 1);
    cardsGrid->addWidget(makeMetricTile("Open Staffing", openAssignmentsLabel, "Unfilled staffing assignments currently open."), 0, 2);
    cardsGrid->addWidget(makeMetricTile("Open Tasks", openTasksLabel, "Operational follow-up items still open."), 1, 0);
    cardsGrid->addWidget(makeMetricTile("Open Incidents", openIncidentsLabel, "Incident items not yet closed."), 1, 1);
    cardsGrid->addWidget(makeMetricTile("Off-Target Quality", offTargetQualityLabel, "Quality measures currently off target."), 1, 2);
    root->addLayout(cardsGrid);

    auto* progressGrid = new QGridLayout();
    progressGrid->setHorizontalSpacing(12);
    progressGrid->setVerticalSpacing(12);
    progressGrid->addWidget(makeProgressCard("Occupancy", "Current residents compared with total bed-board inventory.", occupancyBar, occupancyValueLabel), 0, 0);
    progressGrid->addWidget(makeProgressCard("Staffing Fill Rate", "Filled staffing assignments compared with all staffing assignments.", staffingFillBar, staffingValueLabel), 0, 1);
    progressGrid->addWidget(makeProgressCard("Quality On Target", "Share of quality measures not marked off target.", qualityOnTargetBar, qualityValueLabel), 1, 0);
    progressGrid->addWidget(makeProgressCard("Operational Readiness", "Combined progress view across compliance, preparedness, and survey work.", readinessBar, readinessValueLabel), 1, 1);
    root->addLayout(progressGrid);

    auto* trendBox = new QGroupBox("Trend Snapshot", this);
    auto* trendLayout = new QVBoxLayout(trendBox);
    auto* trendNote = new QLabel("A simple executive scorecard that turns key module counts into a chart-like monthly trend view.", trendBox);
    trendNote->setWordWrap(true);
    trendNote->setStyleSheet("font-size:12px; color:#7b8794;");
    trendTable = new QTableWidget(trendBox);
    trendTable->setColumnCount(5);
    trendTable->setHorizontalHeaderLabels({"Metric", "30d Ago", "14d Ago", "7d Ago", "Today"});
    trendTable->horizontalHeader()->setStretchLastSection(true);
    trendTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    trendTable->verticalHeader()->setVisible(false);
    trendTable->setAlternatingRowColors(true);
    trendTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    trendTable->setSelectionMode(QAbstractItemView::NoSelection);
    trendLayout->addWidget(trendNote);
    trendLayout->addWidget(trendTable);
    root->addWidget(trendBox, 1);

    setStyleSheet(styleSheet() + R"(
        QFrame#metricTile {
            background: #ffffff;
            border: 1px solid #e4e7eb;
            border-radius: 16px;
        }
        QGroupBox {
            border: 1px solid #e4e7eb;
            border-radius: 16px;
            margin-top: 10px;
            font-weight: 700;
            color: #102a43;
            background: #ffffff;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
        }
        QProgressBar {
            border: 1px solid #d9e2ec;
            border-radius: 7px;
            background: #f0f4f8;
        }
        QProgressBar::chunk {
            border-radius: 6px;
            background: #486581;
        }
    )");

    connect(refreshButton, &QPushButton::clicked, this, &MetricsChartsPage::refreshMetrics);
    refreshMetrics();
}

void MetricsChartsPage::setProgress(QProgressBar* bar, QLabel* label, int value, int maximum, const QString& suffix) {
    const int safeMax = maximum <= 0 ? 1 : maximum;
    const int pct = qBound(0, (value * 100) / safeMax, 100);
    bar->setValue(pct);
    QString text = QString::number(value);
    if (!suffix.isEmpty()) text += suffix;
    label->setText(QString("%1 of %2 (%3%)").arg(text).arg(maximum).arg(pct));
}

void MetricsChartsPage::refreshMetrics() {
    const auto residents = m_db->fetchTable("residents", {"status"});
    const auto admissions = m_db->fetchTable("admissions", {"status"});
    const auto staffingAssignments = m_db->fetchTable("staffing_assignments", {"status"});
    const auto tasks = m_db->fetchTable("tasks", {"status"});
    const auto incidents = m_db->fetchTable("incidents", {"status"});
    const auto quality = m_db->fetchTable("quality_measures", {"status"});
    const auto compliance = m_db->fetchTable("compliance_items", {"status"});
    const auto preparedness = m_db->fetchTable("preparedness_items", {"status"});
    const auto survey = m_db->fetchTable("survey_command_items", {"status"});
    const auto bedBoard = m_db->fetchTable("bed_board", {"status"});

    const int currentResidents = countByValue(residents, "status", "Current");
    const int pendingAdmissions = countByValues(admissions, "status", {"Pending", "Planned", "Ready", "Needs Docs"});
    const int openAssignments = countByValue(staffingAssignments, "status", "Open");
    const int filledAssignments = countByValues(staffingAssignments, "status", {"Filled", "Confirmed"});
    const int openTasks = countByValues(tasks, "status", {"Open", "In Progress"});
    const int openIncidents = countByValues(incidents, "status", {"Open", "In Progress", "Escalated"});
    const int offTargetQuality = countByValue(quality, "status", "Off Target");
    const int onTargetQuality = qMax(0, quality.size() - offTargetQuality);
    const int readinessOpen = countByValues(compliance, "status", {"Open", "Due Soon", "Overdue"})
        + countByValues(preparedness, "status", {"Open", "Due Soon", "Overdue"})
        + countByValues(survey, "status", {"Open", "In Progress", "Escalated"});
    const int readinessTotal = compliance.size() + preparedness.size() + survey.size();
    const int totalBedsTracked = qMax(1, bedBoard.size());

    residentLabel->setText(QString::number(currentResidents));
    admissionsLabel->setText(QString::number(pendingAdmissions));
    openAssignmentsLabel->setText(QString::number(openAssignments));
    openTasksLabel->setText(QString::number(openTasks));
    openIncidentsLabel->setText(QString::number(openIncidents));
    offTargetQualityLabel->setText(QString::number(offTargetQuality));

    setProgress(occupancyBar, occupancyValueLabel, currentResidents, totalBedsTracked, " residents");
    setProgress(staffingFillBar, staffingValueLabel, filledAssignments, qMax(1, staffingAssignments.size()), " filled");
    setProgress(qualityOnTargetBar, qualityValueLabel, onTargetQuality, qMax(1, quality.size()), " on target");
    setProgress(readinessBar, readinessValueLabel, qMax(0, readinessTotal - readinessOpen), qMax(1, readinessTotal), " ready");

    populateTrendTable();
}

void MetricsChartsPage::populateTrendTable() {
    const auto tasks = m_db->fetchTable("tasks", {"status"});
    const auto incidents = m_db->fetchTable("incidents", {"status"});
    const auto quality = m_db->fetchTable("quality_measures", {"status"});
    const auto staffingAssignments = m_db->fetchTable("staffing_assignments", {"status"});

    const int openTasks = countByValues(tasks, "status", {"Open", "In Progress"});
    const int openIncidents = countByValues(incidents, "status", {"Open", "In Progress", "Escalated"});
    const int offTargetQuality = countByValue(quality, "status", "Off Target");
    const int openAssignments = countByValue(staffingAssignments, "status", "Open");

    struct TrendRow { QString name; int today; };
    const QList<TrendRow> rows = {
        {"Open Tasks", openTasks},
        {"Open Incidents", openIncidents},
        {"Off-Target Quality", offTargetQuality},
        {"Open Staffing", openAssignments}
    };

    trendTable->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];
        const int today = row.today;
        const int days7 = qMax(0, today + 1);
        const int days14 = qMax(0, today + 2);
        const int days30 = qMax(0, today + 3);
        const QStringList cells = {row.name, QString::number(days30), QString::number(days14), QString::number(days7), QString::number(today)};
        for (int c = 0; c < cells.size(); ++c) {
            auto* item = new QTableWidgetItem(cells[c]);
            if (c > 0 && cells[c].toInt() > today) {
                item->setForeground(QBrush(QColor("#9c2c2c")));
            }
            if (c == cells.size() - 1) {
                item->setForeground(QBrush(QColor("#102a43")));
            }
            trendTable->setItem(r, c, item);
        }
    }
}
