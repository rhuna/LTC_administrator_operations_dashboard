
#pragma once
#include <QWidget>
class DatabaseManager;
class TreatmentsPage : public QWidget {
public:
    explicit TreatmentsPage(DatabaseManager* db, QWidget* parent = nullptr);
};
