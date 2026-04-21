#pragma once
#include <QWidget>
class DatabaseManager;
class DietaryPage : public QWidget {
public:
    explicit DietaryPage(DatabaseManager* db, QWidget* parent = nullptr);
};
