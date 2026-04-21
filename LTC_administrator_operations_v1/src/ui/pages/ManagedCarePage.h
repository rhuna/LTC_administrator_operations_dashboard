#pragma once
#include <QWidget>
class DatabaseManager;
class ManagedCarePage : public QWidget {
public:
    explicit ManagedCarePage(DatabaseManager* db, QWidget* parent = nullptr);
};
