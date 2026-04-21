#pragma once
#include <QWidget>

class DatabaseManager;

class ActivitiesEngagementPage : public QWidget {
    Q_OBJECT
public:
    explicit ActivitiesEngagementPage(DatabaseManager* db, QWidget* parent = nullptr);
};
