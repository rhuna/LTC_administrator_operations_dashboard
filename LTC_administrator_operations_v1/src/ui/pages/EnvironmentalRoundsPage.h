#pragma once
#include <QWidget>
class DatabaseManager;
class EnvironmentalRoundsPage : public QWidget {
public:
    explicit EnvironmentalRoundsPage(DatabaseManager* db, QWidget* parent = nullptr);
};
