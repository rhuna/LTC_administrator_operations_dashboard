#pragma once

#include <QWidget>

class DatabaseManager;
class QSqlQueryModel;
class QTableView;

class SurveyReadinessPage : public QWidget {
    Q_OBJECT
public:
    explicit SurveyReadinessPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
};
