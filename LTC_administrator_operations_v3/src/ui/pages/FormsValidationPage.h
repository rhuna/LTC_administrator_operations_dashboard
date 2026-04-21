#pragma once

#include <QWidget>

class DatabaseManager;
class QTableWidget;

class FormsValidationPage : public QWidget {
    Q_OBJECT
public:
    explicit FormsValidationPage(DatabaseManager* db, QWidget* parent = nullptr);

private:
    DatabaseManager* m_db{nullptr};
    QTableWidget* m_profilesTable{nullptr};
    QTableWidget* m_alertsTable{nullptr};

    void loadProfiles();
    void loadAlerts();
};
