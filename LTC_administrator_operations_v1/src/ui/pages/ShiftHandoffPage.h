#pragma once
#include <QWidget>
class DatabaseManager;
class ShiftHandoffPage : public QWidget {
public:
    explicit ShiftHandoffPage(DatabaseManager* db, QWidget* parent = nullptr);
};
