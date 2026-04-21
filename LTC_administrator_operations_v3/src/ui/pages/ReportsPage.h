#pragma once

#include <QWidget>

class DatabaseManager;
class QTextEdit;
class QPushButton;

class ReportsPage : public QWidget {
    Q_OBJECT
public:
    explicit ReportsPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshPreview();
    void exportDailySummary();
    void exportCensusCsv();
    void exportStaffingCsv();
    void printDailySummary();

private:
    QString buildDailySummaryText() const;
    QString buildCensusCsv() const;
    QString buildStaffingCsv() const;
    bool saveTextToFile(const QString& suggestedName, const QString& filter, const QString& content) const;

    DatabaseManager* m_db{nullptr};
    QTextEdit* m_preview{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QPushButton* m_exportDailyButton{nullptr};
    QPushButton* m_exportCensusButton{nullptr};
    QPushButton* m_exportStaffingButton{nullptr};
    QPushButton* m_printButton{nullptr};
};
