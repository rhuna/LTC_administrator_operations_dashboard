#pragma once
#include <QWidget>
class DatabaseManager;
class HuddlePage : public QWidget {
public:
    explicit HuddlePage(DatabaseManager* db, QWidget* parent = nullptr);
};
