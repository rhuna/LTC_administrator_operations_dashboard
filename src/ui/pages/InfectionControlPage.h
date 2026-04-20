#pragma once
#include <QWidget>
class DatabaseManager;
class InfectionControlPage : public QWidget {
public:
    explicit InfectionControlPage(DatabaseManager* db, QWidget* parent = nullptr);
};
