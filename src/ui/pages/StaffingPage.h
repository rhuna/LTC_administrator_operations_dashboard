#pragma once

#include <QWidget>

class DatabaseManager;
class QSqlQueryModel;
class QTableView;

class StaffingPage : public QWidget {
    Q_OBJECT
public:
    explicit StaffingPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
};
