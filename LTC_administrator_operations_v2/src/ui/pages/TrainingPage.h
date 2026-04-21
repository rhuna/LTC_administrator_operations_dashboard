#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>

class DatabaseManager;

class TrainingPage : public QWidget {
    Q_OBJECT
public:
    explicit TrainingPage(DatabaseManager* db, QWidget* parent = nullptr);

private:
    DatabaseManager* m_db;
    QTableWidget* m_table;
    QLabel* m_summaryLabel;

    void refreshTable();
};
