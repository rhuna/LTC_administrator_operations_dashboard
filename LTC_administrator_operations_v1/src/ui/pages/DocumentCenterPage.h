#pragma once
#include <QWidget>
class DatabaseManager;
class DocumentCenterPage : public QWidget {
public:
    explicit DocumentCenterPage(DatabaseManager* db, QWidget* parent = nullptr);
};
