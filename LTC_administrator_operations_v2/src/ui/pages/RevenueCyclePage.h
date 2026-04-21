#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>

class DatabaseManager;

class RevenueCyclePage : public QWidget {
    Q_OBJECT
public:
    explicit RevenueCyclePage(DatabaseManager* db, QWidget* parent = nullptr);

private:
    DatabaseManager* m_db;
    QTableWidget* m_table;
    QLabel* m_summaryLabel;

    void refreshTable();
};
