#pragma once
#include <QWidget>

class DatabaseManager;

class HousekeepingLaundryPage : public QWidget {
    Q_OBJECT
public:
    explicit HousekeepingLaundryPage(DatabaseManager* db, QWidget* parent = nullptr);
};
