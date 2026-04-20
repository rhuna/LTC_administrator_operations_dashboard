#pragma once
#include <QWidget>
class DatabaseManager;
class RiskManagementPage : public QWidget {
public:
    explicit RiskManagementPage(DatabaseManager* db, QWidget* parent = nullptr);
};
