#include "QualityMeasuresPage.h"
#include "../../data/DatabaseManager.h"

#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {
QFrame* makeCard(const QString& title, QLabel** valueLabel) {
    auto* frame = new QFrame();
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setStyleSheet("QFrame { background: #f7f9fc; border: 1px solid #d9e2ec; border-radius: 12px; } QLabel[role='title'] { color: #52606d; font-size: 12px; } QLabel[role='value'] { color: #102a43; font-size: 24px; font-weight: 700; }");
    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(14, 12, 14, 12);
    auto* titleLabel = new QLabel(title, frame);
    titleLabel->setProperty("role", "title");
    auto* numberLabel = new QLabel("0", frame);
    numberLabel->setProperty("role", "value");
    layout->addWidget(titleLabel);
    layout->addWidget(numberLabel);
    *valueLabel = numberLabel;
    return frame;
}
}

QualityMeasuresPage::QualityMeasuresPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(14);

    auto* heading = new QLabel("Quality Measures Analytics Hub", this);
    heading->setStyleSheet("font-size: 22px; font-weight: 700; color: #102a43;");
    root->addWidget(heading);

    auto* intro = new QLabel("Track current measure performance, monthly trend movement, active follow-up work, and driver themes in one clinical quality workspace.", this);
    intro->setWordWrap(true);
    intro->setStyleSheet("color: #52606d; margin-bottom: 4px;");
    root->addWidget(intro);

    auto* summaryLayout = new QGridLayout();
    summaryLayout->setHorizontalSpacing(12);
    summaryLayout->setVerticalSpacing(12);
    summaryLayout->addWidget(makeCard("Measures", &m_totalMeasuresLabel), 0, 0);
    summaryLayout->addWidget(makeCard("Off Target", &m_offTargetLabel), 0, 1);
    summaryLayout->addWidget(makeCard("Watch", &m_watchLabel), 0, 2);
    summaryLayout->addWidget(makeCard("Open Follow-Ups", &m_openFollowupsLabel), 0, 3);
    summaryLayout->addWidget(makeCard("Trend Snapshots", &m_snapshotRowsLabel), 1, 0);
    summaryLayout->addWidget(makeCard("Driver Themes", &m_focusAreasLabel), 1, 1);
    root->addLayout(summaryLayout);

    auto* measuresBox = new QGroupBox("Measure performance overview", this);
    auto* measuresLayout = new QVBoxLayout(measuresBox);
    m_measureTable = new QTableWidget(this);
    m_measureTable->setColumnCount(7);
    m_measureTable->setHorizontalHeaderLabels({"Measure", "Category", "Current", "Target", "Trend", "Status", "Notes"});
    m_measureTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_measureTable->setAlternatingRowColors(true);
    measuresLayout->addWidget(m_measureTable);
    root->addWidget(measuresBox);

    auto* midRow = new QGridLayout();
    midRow->setHorizontalSpacing(12);
    midRow->setVerticalSpacing(12);

    auto* snapshotBox = new QGroupBox("Monthly trend snapshots", this);
    auto* snapshotLayout = new QVBoxLayout(snapshotBox);
    m_snapshotTable = new QTableWidget(this);
    m_snapshotTable->setColumnCount(7);
    m_snapshotTable->setHorizontalHeaderLabels({"Month", "Measure", "Current", "Target", "Variance", "Trend", "Status"});
    m_snapshotTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_snapshotTable->setAlternatingRowColors(true);
    snapshotLayout->addWidget(m_snapshotTable);
    root->addWidget(snapshotBox);

    auto* lowerLayout = new QGridLayout();
    lowerLayout->setHorizontalSpacing(12);
    lowerLayout->setVerticalSpacing(12);

    auto* addMeasureBox = new QGroupBox("Add / update measure snapshot", this);
    auto* addMeasureLayout = new QFormLayout(addMeasureBox);
    m_measureNameEdit = new QLineEdit(this);
    m_categoryEdit = new QLineEdit(this);
    m_currentValueEdit = new QLineEdit(this);
    m_targetValueEdit = new QLineEdit(this);
    m_trendEdit = new QLineEdit(this);
    m_statusEdit = new QLineEdit(this);
    m_measureNotesEdit = new QTextEdit(this);
    m_measureNotesEdit->setMinimumHeight(90);
    addMeasureLayout->addRow("Measure", m_measureNameEdit);
    addMeasureLayout->addRow("Category", m_categoryEdit);
    addMeasureLayout->addRow("Current", m_currentValueEdit);
    addMeasureLayout->addRow("Target", m_targetValueEdit);
    addMeasureLayout->addRow("Trend", m_trendEdit);
    addMeasureLayout->addRow("Status", m_statusEdit);
    addMeasureLayout->addRow("Notes", m_measureNotesEdit);
    auto* addMeasureButton = new QPushButton("Save measure", this);
    addMeasureLayout->addRow(addMeasureButton);
    lowerLayout->addWidget(addMeasureBox, 0, 0);

    auto* followupBox = new QGroupBox("Quality follow-up worklist", this);
    auto* followupLayout = new QVBoxLayout(followupBox);
    m_followupTable = new QTableWidget(this);
    m_followupTable->setColumnCount(6);
    m_followupTable->setHorizontalHeaderLabels({"Measure", "Focus Area", "Owner", "Due", "Status", "Action Step"});
    m_followupTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_followupTable->setAlternatingRowColors(true);
    followupLayout->addWidget(m_followupTable);

    auto* addFollowupForm = new QFormLayout();
    m_followupMeasureEdit = new QLineEdit(this);
    m_focusAreaEdit = new QLineEdit(this);
    m_ownerEdit = new QLineEdit(this);
    m_dueDateEdit = new QLineEdit(this);
    m_followupStatusEdit = new QLineEdit(this);
    m_actionStepEdit = new QTextEdit(this);
    m_actionStepEdit->setMinimumHeight(70);
    addFollowupForm->addRow("Measure", m_followupMeasureEdit);
    addFollowupForm->addRow("Focus Area", m_focusAreaEdit);
    addFollowupForm->addRow("Owner", m_ownerEdit);
    addFollowupForm->addRow("Due Date", m_dueDateEdit);
    addFollowupForm->addRow("Status", m_followupStatusEdit);
    addFollowupForm->addRow("Action Step", m_actionStepEdit);
    auto* addFollowupButton = new QPushButton("Add follow-up", this);
    addFollowupForm->addRow(addFollowupButton);
    followupLayout->addLayout(addFollowupForm);
    lowerLayout->addWidget(followupBox, 0, 1);

    auto* trendInputBox = new QGroupBox("Add monthly trend row", this);
    auto* trendInputLayout = new QFormLayout(trendInputBox);
    m_snapshotMonthEdit = new QLineEdit(this);
    m_snapshotMeasureEdit = new QLineEdit(this);
    m_snapshotCurrentEdit = new QLineEdit(this);
    m_snapshotTargetEdit = new QLineEdit(this);
    m_snapshotVarianceEdit = new QLineEdit(this);
    m_snapshotTrendEdit = new QLineEdit(this);
    m_snapshotStatusEdit = new QLineEdit(this);
    trendInputLayout->addRow("Month", m_snapshotMonthEdit);
    trendInputLayout->addRow("Measure", m_snapshotMeasureEdit);
    trendInputLayout->addRow("Current", m_snapshotCurrentEdit);
    trendInputLayout->addRow("Target", m_snapshotTargetEdit);
    trendInputLayout->addRow("Variance", m_snapshotVarianceEdit);
    trendInputLayout->addRow("Trend", m_snapshotTrendEdit);
    trendInputLayout->addRow("Status", m_snapshotStatusEdit);
    auto* addTrendButton = new QPushButton("Add trend row", this);
    trendInputLayout->addRow(addTrendButton);
    lowerLayout->addWidget(trendInputBox, 1, 0);

    auto* driverBox = new QGroupBox("Quality driver themes", this);
    auto* driverLayout = new QVBoxLayout(driverBox);
    m_focusAreaTable = new QTableWidget(this);
    m_focusAreaTable->setColumnCount(6);
    m_focusAreaTable->setHorizontalHeaderLabels({"Focus Area", "Driver", "Owner", "Status", "Next Review", "Notes"});
    m_focusAreaTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_focusAreaTable->setAlternatingRowColors(true);
    driverLayout->addWidget(m_focusAreaTable);

    auto* driverForm = new QFormLayout();
    m_driverFocusEdit = new QLineEdit(this);
    m_driverTextEdit = new QLineEdit(this);
    m_driverOwnerEdit = new QLineEdit(this);
    m_driverStatusEdit = new QLineEdit(this);
    m_driverReviewEdit = new QLineEdit(this);
    m_driverNotesEdit = new QTextEdit(this);
    m_driverNotesEdit->setMinimumHeight(70);
    driverForm->addRow("Focus Area", m_driverFocusEdit);
    driverForm->addRow("Driver", m_driverTextEdit);
    driverForm->addRow("Owner", m_driverOwnerEdit);
    driverForm->addRow("Status", m_driverStatusEdit);
    driverForm->addRow("Next Review", m_driverReviewEdit);
    driverForm->addRow("Notes", m_driverNotesEdit);
    auto* addDriverButton = new QPushButton("Add driver theme", this);
    driverForm->addRow(addDriverButton);
    driverLayout->addLayout(driverForm);
    lowerLayout->addWidget(driverBox, 1, 1);

    root->addLayout(lowerLayout);

    connect(addMeasureButton, &QPushButton::clicked, this, [this]() {
        if (!m_db) return;
        m_db->addRecord("quality_measures", {
            {"measure_name", m_measureNameEdit->text().trimmed()},
            {"category", m_categoryEdit->text().trimmed()},
            {"current_value", m_currentValueEdit->text().trimmed()},
            {"target_value", m_targetValueEdit->text().trimmed()},
            {"trend", m_trendEdit->text().trimmed()},
            {"status", m_statusEdit->text().trimmed()},
            {"notes", m_measureNotesEdit->toPlainText().trimmed()}
        });
        m_measureNameEdit->clear();
        m_categoryEdit->clear();
        m_currentValueEdit->clear();
        m_targetValueEdit->clear();
        m_trendEdit->clear();
        m_statusEdit->clear();
        m_measureNotesEdit->clear();
        refreshView();
    });

    connect(addFollowupButton, &QPushButton::clicked, this, [this]() {
        if (!m_db) return;
        m_db->addRecord("quality_followups", {
            {"measure_name", m_followupMeasureEdit->text().trimmed()},
            {"focus_area", m_focusAreaEdit->text().trimmed()},
            {"owner", m_ownerEdit->text().trimmed()},
            {"due_date", m_dueDateEdit->text().trimmed()},
            {"status", m_followupStatusEdit->text().trimmed()},
            {"action_step", m_actionStepEdit->toPlainText().trimmed()}
        });
        m_followupMeasureEdit->clear();
        m_focusAreaEdit->clear();
        m_ownerEdit->clear();
        m_dueDateEdit->clear();
        m_followupStatusEdit->clear();
        m_actionStepEdit->clear();
        refreshView();
    });

    connect(addTrendButton, &QPushButton::clicked, this, [this]() {
        if (!m_db) return;
        m_db->addRecord("quality_monthly_snapshots", {
            {"month_label", m_snapshotMonthEdit->text().trimmed()},
            {"measure_name", m_snapshotMeasureEdit->text().trimmed()},
            {"current_value", m_snapshotCurrentEdit->text().trimmed()},
            {"target_value", m_snapshotTargetEdit->text().trimmed()},
            {"variance_note", m_snapshotVarianceEdit->text().trimmed()},
            {"trend", m_snapshotTrendEdit->text().trimmed()},
            {"status", m_snapshotStatusEdit->text().trimmed()}
        });
        m_snapshotMonthEdit->clear();
        m_snapshotMeasureEdit->clear();
        m_snapshotCurrentEdit->clear();
        m_snapshotTargetEdit->clear();
        m_snapshotVarianceEdit->clear();
        m_snapshotTrendEdit->clear();
        m_snapshotStatusEdit->clear();
        refreshView();
    });

    connect(addDriverButton, &QPushButton::clicked, this, [this]() {
        if (!m_db) return;
        m_db->addRecord("quality_focus_areas", {
            {"focus_area", m_driverFocusEdit->text().trimmed()},
            {"driver", m_driverTextEdit->text().trimmed()},
            {"owner", m_driverOwnerEdit->text().trimmed()},
            {"status", m_driverStatusEdit->text().trimmed()},
            {"next_review", m_driverReviewEdit->text().trimmed()},
            {"notes", m_driverNotesEdit->toPlainText().trimmed()}
        });
        m_driverFocusEdit->clear();
        m_driverTextEdit->clear();
        m_driverOwnerEdit->clear();
        m_driverStatusEdit->clear();
        m_driverReviewEdit->clear();
        m_driverNotesEdit->clear();
        refreshView();
    });

    refreshView();
}

void QualityMeasuresPage::refreshView() {
    populateMeasures();
    populateFollowups();
    populateSnapshots();
    populateFocusAreas();
    updateSummary();
}

void QualityMeasuresPage::populateMeasures() {
    const QStringList cols = {"measure_name", "category", "current_value", "target_value", "trend", "status", "notes"};
    const auto rows = m_db ? m_db->fetchTable("quality_measures", cols) : QList<QMap<QString, QString>>{};
    m_measureTable->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_measureTable->rowCount();
        m_measureTable->insertRow(r);
        for (int i = 0; i < cols.size(); ++i) {
            m_measureTable->setItem(r, i, new QTableWidgetItem(row.value(cols[i])));
        }
    }
}

void QualityMeasuresPage::populateFollowups() {
    const QStringList cols = {"measure_name", "focus_area", "owner", "due_date", "status", "action_step"};
    const auto rows = m_db ? m_db->fetchTable("quality_followups", cols) : QList<QMap<QString, QString>>{};
    m_followupTable->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_followupTable->rowCount();
        m_followupTable->insertRow(r);
        for (int i = 0; i < cols.size(); ++i) {
            m_followupTable->setItem(r, i, new QTableWidgetItem(row.value(cols[i])));
        }
    }
}

void QualityMeasuresPage::populateSnapshots() {
    const QStringList cols = {"month_label", "measure_name", "current_value", "target_value", "variance_note", "trend", "status"};
    const auto rows = m_db ? m_db->fetchTable("quality_monthly_snapshots", cols) : QList<QMap<QString, QString>>{};
    m_snapshotTable->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_snapshotTable->rowCount();
        m_snapshotTable->insertRow(r);
        for (int i = 0; i < cols.size(); ++i) {
            m_snapshotTable->setItem(r, i, new QTableWidgetItem(row.value(cols[i])));
        }
    }
}

void QualityMeasuresPage::populateFocusAreas() {
    const QStringList cols = {"focus_area", "driver", "owner", "status", "next_review", "notes"};
    const auto rows = m_db ? m_db->fetchTable("quality_focus_areas", cols) : QList<QMap<QString, QString>>{};
    m_focusAreaTable->setRowCount(0);
    for (const auto& row : rows) {
        const int r = m_focusAreaTable->rowCount();
        m_focusAreaTable->insertRow(r);
        for (int i = 0; i < cols.size(); ++i) {
            m_focusAreaTable->setItem(r, i, new QTableWidgetItem(row.value(cols[i])));
        }
    }
}

void QualityMeasuresPage::updateSummary() {
    const auto measures = m_db ? m_db->fetchTable("quality_measures", {"status"}) : QList<QMap<QString, QString>>{};
    const auto followups = m_db ? m_db->fetchTable("quality_followups", {"status"}) : QList<QMap<QString, QString>>{};
    const auto snapshots = m_db ? m_db->fetchTable("quality_monthly_snapshots", {"status"}) : QList<QMap<QString, QString>>{};
    const auto focusAreas = m_db ? m_db->fetchTable("quality_focus_areas", {"status"}) : QList<QMap<QString, QString>>{};
    int offTarget = 0;
    int watch = 0;
    int openFollowups = 0;
    for (const auto& row : measures) {
        const auto status = row.value("status").trimmed();
        if (status.compare("Off Target", Qt::CaseInsensitive) == 0) ++offTarget;
        if (status.compare("Watch", Qt::CaseInsensitive) == 0) ++watch;
    }
    for (const auto& row : followups) {
        const auto status = row.value("status").trimmed();
        if (status.compare("Closed", Qt::CaseInsensitive) != 0 && status.compare("Complete", Qt::CaseInsensitive) != 0) ++openFollowups;
    }
    m_totalMeasuresLabel->setText(QString::number(measures.size()));
    m_offTargetLabel->setText(QString::number(offTarget));
    m_watchLabel->setText(QString::number(watch));
    m_openFollowupsLabel->setText(QString::number(openFollowups));
    m_snapshotRowsLabel->setText(QString::number(snapshots.size()));
    m_focusAreasLabel->setText(QString::number(focusAreas.size()));
}
