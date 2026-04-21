#pragma once
#include <QWidget>

class DatabaseManager;

class SocialServicesPage : public QWidget {
    Q_OBJECT
public:
    explicit SocialServicesPage(DatabaseManager* db, QWidget* parent = nullptr);
};
