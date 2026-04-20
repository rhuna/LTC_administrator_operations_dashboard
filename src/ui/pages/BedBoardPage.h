#pragma once
#include <QWidget>
class DatabaseManager;
class BedBoardPage : public QWidget {
public:
    explicit BedBoardPage(DatabaseManager* db, QWidget* parent = nullptr);
};
