#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QTextEdit;
class QTableWidget;

class QualityMeasuresPage : public QWidget {
public:
    explicit QualityMeasuresPage(DatabaseManager* db, QWidget* parent = nullptr);

private:
    void refreshView();
    void populateMeasures();
    void populateFollowups();
    void populateSnapshots();
    void populateFocusAreas();
    void updateSummary();

    DatabaseManager* m_db{nullptr};
    QLabel* m_totalMeasuresLabel{nullptr};
    QLabel* m_offTargetLabel{nullptr};
    QLabel* m_watchLabel{nullptr};
    QLabel* m_openFollowupsLabel{nullptr};
    QLabel* m_snapshotRowsLabel{nullptr};
    QLabel* m_focusAreasLabel{nullptr};
    QTableWidget* m_measureTable{nullptr};
    QTableWidget* m_followupTable{nullptr};
    QTableWidget* m_snapshotTable{nullptr};
    QTableWidget* m_focusAreaTable{nullptr};
    QLineEdit* m_measureNameEdit{nullptr};
    QLineEdit* m_categoryEdit{nullptr};
    QLineEdit* m_currentValueEdit{nullptr};
    QLineEdit* m_targetValueEdit{nullptr};
    QLineEdit* m_trendEdit{nullptr};
    QLineEdit* m_statusEdit{nullptr};
    QTextEdit* m_measureNotesEdit{nullptr};
    QLineEdit* m_followupMeasureEdit{nullptr};
    QLineEdit* m_focusAreaEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QLineEdit* m_followupStatusEdit{nullptr};
    QTextEdit* m_actionStepEdit{nullptr};
    QLineEdit* m_snapshotMonthEdit{nullptr};
    QLineEdit* m_snapshotMeasureEdit{nullptr};
    QLineEdit* m_snapshotCurrentEdit{nullptr};
    QLineEdit* m_snapshotTargetEdit{nullptr};
    QLineEdit* m_snapshotVarianceEdit{nullptr};
    QLineEdit* m_snapshotTrendEdit{nullptr};
    QLineEdit* m_snapshotStatusEdit{nullptr};
    QLineEdit* m_driverFocusEdit{nullptr};
    QLineEdit* m_driverTextEdit{nullptr};
    QLineEdit* m_driverOwnerEdit{nullptr};
    QLineEdit* m_driverStatusEdit{nullptr};
    QLineEdit* m_driverReviewEdit{nullptr};
    QTextEdit* m_driverNotesEdit{nullptr};
};
