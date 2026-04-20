#pragma once
#include <QWidget>
class DatabaseManager;
class DashboardPage : public QWidget {
public:
    explicit DashboardPage(DatabaseManager* db, QWidget* parent = nullptr);
};
