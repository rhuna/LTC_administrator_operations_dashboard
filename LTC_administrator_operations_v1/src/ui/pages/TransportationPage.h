#pragma once
#include <QWidget>
class DatabaseManager;
class TransportationPage : public QWidget {
public:
    explicit TransportationPage(DatabaseManager* db, QWidget* parent = nullptr);
};
