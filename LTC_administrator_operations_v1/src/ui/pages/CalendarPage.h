#pragma once
#include <QWidget>

class DatabaseManager;

class CalendarPage : public QWidget {
public:
    explicit CalendarPage(DatabaseManager* db, QWidget* parent = nullptr);
};
