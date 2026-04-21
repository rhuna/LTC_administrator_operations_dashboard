#pragma once
#include <QWidget>
class DatabaseManager;
class CompliancePage : public QWidget {
public:
    explicit CompliancePage(DatabaseManager* db, QWidget* parent = nullptr);
};
