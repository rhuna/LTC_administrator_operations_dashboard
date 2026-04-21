#pragma once
#include <QWidget>
class DatabaseManager;
class MdsTripleCheckPage : public QWidget {
public:
    explicit MdsTripleCheckPage(DatabaseManager* db, QWidget* parent = nullptr);
};
