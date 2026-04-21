#pragma once
#include <QWidget>
class DatabaseManager;
class CareConferencePage : public QWidget {
public:
    explicit CareConferencePage(DatabaseManager* db, QWidget* parent = nullptr);
};
