#include "KpiTrendEnginePage.h"
#include "../../data/DatabaseManager.h"

#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
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
QFrame* makeStatCard(const QString& title, QLabel*& valueLabel, const QString& helper, QWidget* parent) {
    auto* card = new QFrame(parent);
    card->setObjectName("kpiTrendStatCard");
    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(4);

    auto* titleLabel = new QLabel(title, card);
    titleLabel->setStyleSheet("font-size:12px; font-weight:700; color:#486581;");

    valueLabel = new QLabel("0", card);
    valueLabel->setStyleSheet("font-size:24px; font-weight:800; color:#102a43;");

    auto* helperLabel = new QLabel(helper, card);
    helperLabel->setWordWrap(true);
    helperLabel->setStyleSheet("font-size:12px; color:#7b8794;");

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    layout->addWidget(helperLabel);
    return card;
}
}

KpiTrendEnginePage::KpiTrendEnginePage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(14);

    auto* title = new QLabel("KPI Trend Engine", this);
    title->setStyleSheet("font-size:24px; font-weight:800; color:#102a43;");
    auto* subtitle = new QLabel("Track executive KPIs over time with trend rows, owners, and simple off-track visibility for the most important building-level metrics.", this);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("font-size:13px; color:#486581;");

    auto* headRow = new QHBoxLayout();
    auto* titleWrap = new QVBoxLayout();
    titleWrap->addWidget(title);
    titleWrap->addWidget(subtitle);
    auto* refreshButton = new QPushButton("Refresh", this);
    refreshButton->setStyleSheet("padding:8px 14px; font-weight:600;");
    headRow->addLayout(titleWrap, 1);
    headRow->addWidget(refreshButton, 0, Qt::AlignTop);
    root->addLayout(headRow);

    auto* statGrid = new QGridLayout();
    statGrid->setHorizontalSpacing(12);
    statGrid->setVerticalSpacing(12);
    statGrid->addWidget(makeStatCard("Trend rows", totalRowsLabel, "Saved KPI trend snapshots available for review.", this), 0, 0);
    statGrid->addWidget(makeStatCard("Off track", offTrackLabel, "Rows marked off track and needing leadership attention.", this), 0, 1);
    statGrid->addWidget(makeStatCard("Watch", watchLabel, "Rows marked watch before they become an active miss.", this), 0, 2);
    statGrid->addWidget(makeStatCard("Measures tracked", measuresLabel, "Distinct KPI measures currently represented.", this), 0, 3);
    root->addLayout(statGrid);

    auto* formBox = new QGroupBox("Add KPI Trend Row", this);
    auto* formLayout = new QFormLayout(formBox);
    metricEdit = new QLineEdit(formBox);
    metricEdit->setPlaceholderText("Example: Occupancy");
    periodEdit = new QLineEdit(formBox);
    periodEdit->setPlaceholderText("Example: 2026-04");
    valueEdit = new QLineEdit(formBox);
    valueEdit->setPlaceholderText("Example: 91.2%");
    targetEdit = new QLineEdit(formBox);
    targetEdit->setPlaceholderText("Example: >= 92.0%");
    statusEdit = new QLineEdit(formBox);
    statusEdit->setPlaceholderText("On Track / Watch / Off Track");
    ownerEdit = new QLineEdit(formBox);
    ownerEdit->setPlaceholderText("Owner");
    noteEdit = new QLineEdit(formBox);
    noteEdit->setPlaceholderText("Short note");

    formLayout->addRow("Metric", metricEdit);
    formLayout->addRow("Period", periodEdit);
    formLayout->addRow("Value", valueEdit);
    formLayout->addRow("Target", targetEdit);
    formLayout->addRow("Status", statusEdit);
    formLayout->addRow("Owner", ownerEdit);
    formLayout->addRow("Note", noteEdit);

    auto* buttonRow = new QHBoxLayout();
    auto* addButton = new QPushButton("Add Trend Row", formBox);
    buttonRow->addWidget(addButton);
    buttonRow->addStretch();
    formLayout->addRow(buttonRow);
    root->addWidget(formBox);

    auto* tableBox = new QGroupBox("Trend Review", this);
    auto* tableLayout = new QVBoxLayout(tableBox);
    auto* note = new QLabel("Use this workspace to keep a compact rolling history of executive KPIs like occupancy, staffing fill rate, quality on-target rate, and reimbursement readiness.", tableBox);
    note->setWordWrap(true);
    note->setStyleSheet("font-size:12px; color:#7b8794;");
    tableWidget = new QTableWidget(tableBox);
    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderLabels({"Metric", "Period", "Value", "Target", "Status", "Owner", "Note"});
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableLayout->addWidget(note);
    tableLayout->addWidget(tableWidget);
    root->addWidget(tableBox, 1);

    setStyleSheet(styleSheet() + R"(
        QFrame#kpiTrendStatCard {
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
    )");

    connect(refreshButton, &QPushButton::clicked, this, &KpiTrendEnginePage::refreshView);
    connect(addButton, &QPushButton::clicked, this, &KpiTrendEnginePage::addTrendRow);
    refreshView();
}

void KpiTrendEnginePage::refreshView() {
    const auto rows = m_db->fetchTable("kpi_trend_rows", {"metric_name", "period_label", "metric_value", "target_value", "trend_status", "owner_name", "note_text"});
    totalRowsLabel->setText(QString::number(rows.size()));

    int offTrack = 0;
    int watch = 0;
    QStringList measureNames;
    for (const auto& row : rows) {
        const QString status = row.value("trend_status");
        if (status.compare("Off Track", Qt::CaseInsensitive) == 0) ++offTrack;
        if (status.compare("Watch", Qt::CaseInsensitive) == 0) ++watch;
        const QString metric = row.value("metric_name");
        if (!metric.isEmpty() && !measureNames.contains(metric)) measureNames << metric;
    }
    offTrackLabel->setText(QString::number(offTrack));
    watchLabel->setText(QString::number(watch));
    measuresLabel->setText(QString::number(measureNames.size()));

    tableWidget->setRowCount(rows.size());
    const QStringList cols = {"metric_name", "period_label", "metric_value", "target_value", "trend_status", "owner_name", "note_text"};
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < cols.size(); ++c) {
            auto* item = new QTableWidgetItem(rows[r].value(cols[c]));
            tableWidget->setItem(r, c, item);
        }
    }
}

void KpiTrendEnginePage::addTrendRow() {
    QMap<QString, QString> values;
    values.insert("metric_name", metricEdit->text().trimmed());
    values.insert("period_label", periodEdit->text().trimmed());
    values.insert("metric_value", valueEdit->text().trimmed());
    values.insert("target_value", targetEdit->text().trimmed());
    values.insert("trend_status", statusEdit->text().trimmed());
    values.insert("owner_name", ownerEdit->text().trimmed());
    values.insert("note_text", noteEdit->text().trimmed());

    if (!m_db->addRecord("kpi_trend_rows", values)) return;

    metricEdit->clear();
    periodEdit->clear();
    valueEdit->clear();
    targetEdit->clear();
    statusEdit->clear();
    ownerEdit->clear();
    noteEdit->clear();
    refreshView();
}
