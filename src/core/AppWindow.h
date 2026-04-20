#pragma once
#include <QMainWindow>
class DatabaseManager;
class AppWindow : public QMainWindow {
public:
    explicit AppWindow(DatabaseManager* db, QWidget* parent = nullptr);
};
