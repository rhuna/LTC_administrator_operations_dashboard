#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>

class DatabaseManager;

class ContractManagementPage : public QWidget {
    Q_OBJECT
public:
    explicit ContractManagementPage(DatabaseManager* db, QWidget* parent = nullptr);

private:
    DatabaseManager* m_db;
    QTableWidget* m_table;
    QLabel* m_summaryLabel;

    void refreshTable();
};
