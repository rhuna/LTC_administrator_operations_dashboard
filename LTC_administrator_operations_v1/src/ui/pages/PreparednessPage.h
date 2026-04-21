#pragma once
#include <QWidget>
class DatabaseManager;
class PreparednessPage : public QWidget {
public:
    explicit PreparednessPage(DatabaseManager* db, QWidget* parent = nullptr);
};
