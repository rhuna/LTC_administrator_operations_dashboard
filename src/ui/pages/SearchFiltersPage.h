#pragma once
#include <QWidget>

class DatabaseManager;
class QComboBox;
class QLineEdit;
class QTableWidget;
class QLabel;
class QPushButton;

class SearchFiltersPage : public QWidget {
    Q_OBJECT
public:
    explicit SearchFiltersPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void applyFilters();
    void resetFilters();

private:
    DatabaseManager* m_db{nullptr};
    QComboBox* m_moduleCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QLineEdit* m_keywordEdit{nullptr};
    QTableWidget* m_table{nullptr};
    QLabel* m_summaryLabel{nullptr};

    void populateTable();
};
