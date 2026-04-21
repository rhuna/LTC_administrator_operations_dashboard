#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QProgressBar;
class QTableWidget;

class MetricsChartsPage : public QWidget {
    Q_OBJECT
public:
    explicit MetricsChartsPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshMetrics();

private:
    void populateTrendTable();
    void setProgress(QProgressBar* bar, QLabel* label, int value, int maximum, const QString& suffix = QString());

    DatabaseManager* m_db {nullptr};

    QLabel* residentLabel {nullptr};
    QLabel* admissionsLabel {nullptr};
    QLabel* openAssignmentsLabel {nullptr};
    QLabel* openTasksLabel {nullptr};
    QLabel* openIncidentsLabel {nullptr};
    QLabel* offTargetQualityLabel {nullptr};

    QProgressBar* occupancyBar {nullptr};
    QProgressBar* staffingFillBar {nullptr};
    QProgressBar* qualityOnTargetBar {nullptr};
    QProgressBar* readinessBar {nullptr};

    QLabel* occupancyValueLabel {nullptr};
    QLabel* staffingValueLabel {nullptr};
    QLabel* qualityValueLabel {nullptr};
    QLabel* readinessValueLabel {nullptr};

    QTableWidget* trendTable {nullptr};
};
