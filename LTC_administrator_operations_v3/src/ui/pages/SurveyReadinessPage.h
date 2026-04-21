#pragma once
#include <QWidget>
class DatabaseManager;
class SurveyReadinessPage : public QWidget {
public:
    explicit SurveyReadinessPage(DatabaseManager* db, QWidget* parent = nullptr);
};
