#pragma once
#include <QWidget>
class DatabaseManager;
class QTableWidget;
class QLabel;

class AuditLogPage : public QWidget {
    Q_OBJECT
public:
    explicit AuditLogPage(DatabaseManager* db, QWidget* parent = nullptr);
private:
    DatabaseManager* m_db;
    QLabel* m_summaryLabel;
    QTableWidget* m_tableWidget;
};
