#pragma once
#include <QWidget>
class DatabaseManager;
class PharmacyPage : public QWidget {
public:
    explicit PharmacyPage(DatabaseManager* db, QWidget* parent = nullptr);
};
