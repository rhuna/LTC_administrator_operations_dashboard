#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QSqlQueryModel;
class QTableView;

class TasksPage : public QWidget {
    Q_OBJECT
public:
    explicit TasksPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddTaskClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QLineEdit* titleEdit_;
    QLineEdit* ownerEdit_;
    QComboBox* priorityCombo_;
    QDateEdit* dueDateEdit_;
};
