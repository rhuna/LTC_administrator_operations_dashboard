#pragma once
#include <QWidget>
class DatabaseManager;
class BudgetPage : public QWidget {
public:
    explicit BudgetPage(DatabaseManager* db, QWidget* parent = nullptr);
};
