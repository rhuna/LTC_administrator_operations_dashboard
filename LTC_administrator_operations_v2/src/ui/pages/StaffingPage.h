
#pragma once
#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

class StaffingPage : public QWidget {
    Q_OBJECT
public:
    explicit StaffingPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshTables();
    void handleAddNumbers();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_snapshotLabel{nullptr};
    QLabel* m_latestCensusLabel{nullptr};
    QLabel* m_totalNursingLabel{nullptr};
    QLabel* m_cnaRatioLabel{nullptr};
    QLabel* m_licensedRatioLabel{nullptr};
    QLabel* m_totalRatioLabel{nullptr};

    QLineEdit* m_dateEdit{nullptr};
    QLineEdit* m_shiftEdit{nullptr};
    QLineEdit* m_censusEdit{nullptr};
    QLineEdit* m_rnEdit{nullptr};
    QLineEdit* m_lpnEdit{nullptr};
    QLineEdit* m_cnaEdit{nullptr};
    QLineEdit* m_agencyEdit{nullptr};
    QLineEdit* m_notesEdit{nullptr};

    QPushButton* m_addButton{nullptr};
    QPushButton* m_refreshButton{nullptr};

    QTableWidget* m_entriesTable{nullptr};
    QTableWidget* m_ratioTable{nullptr};
    QTableWidget* m_minimumTable{nullptr};
};
