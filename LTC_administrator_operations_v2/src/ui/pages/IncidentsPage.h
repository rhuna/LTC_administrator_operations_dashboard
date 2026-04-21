#pragma once
#include <QWidget>
class DatabaseManager;
class IncidentsPage : public QWidget {
public:
    explicit IncidentsPage(DatabaseManager* db, QWidget* parent = nullptr);
};
