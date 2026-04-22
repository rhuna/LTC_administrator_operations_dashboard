#pragma once
#include <QWidget>

class DatabaseManager;

class SurveyCommandCenterPage : public QWidget {
public:
    explicit SurveyCommandCenterPage(DatabaseManager* db, QWidget* parent = nullptr);
};
