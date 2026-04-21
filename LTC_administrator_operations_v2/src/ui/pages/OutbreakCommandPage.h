#pragma once
#include <QWidget>
class DatabaseManager;
class OutbreakCommandPage : public QWidget {
public:
    explicit OutbreakCommandPage(DatabaseManager* db, QWidget* parent = nullptr);
};
