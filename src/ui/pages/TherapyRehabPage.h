#pragma once
#include <QWidget>

class DatabaseManager;

class TherapyRehabPage : public QWidget {
    Q_OBJECT
public:
    explicit TherapyRehabPage(DatabaseManager* db, QWidget* parent = nullptr);
};
