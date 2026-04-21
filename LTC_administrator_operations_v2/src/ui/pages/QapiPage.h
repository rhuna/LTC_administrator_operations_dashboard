#pragma once
#include <QWidget>
class DatabaseManager;
class QapiPage : public QWidget {
public:
    explicit QapiPage(DatabaseManager* db, QWidget* parent = nullptr);
};
