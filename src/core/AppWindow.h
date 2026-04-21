#pragma once
#include <QMainWindow>

class DatabaseManager;

class AppWindow : public QMainWindow {
public:
    explicit AppWindow(DatabaseManager* db, const QString& fullName, const QString& roleName, QWidget* parent = nullptr);
};
