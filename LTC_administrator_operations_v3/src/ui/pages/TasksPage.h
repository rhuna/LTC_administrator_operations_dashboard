#pragma once
#include <QWidget>
class DatabaseManager;
class TasksPage : public QWidget {
public:
    explicit TasksPage(DatabaseManager* db, QWidget* parent = nullptr);
};
