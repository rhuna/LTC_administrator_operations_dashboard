#pragma once
#include <QWidget>
class DatabaseManager;
class AlertsPage : public QWidget {
public:
    explicit AlertsPage(DatabaseManager* db, QWidget* parent = nullptr);
};
