#pragma once

#include <QWidget>

class DatabaseManager;
class QTableView;
class QSqlQueryModel;

class ResidentsPage : public QWidget {
    Q_OBJECT
public:
    explicit ResidentsPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
};
