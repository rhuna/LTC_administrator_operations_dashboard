
#pragma once
#include <QWidget>
class DatabaseManager;
class QLineEdit;
class QPushButton;
class QTableWidget;

class TreatmentsPage : public QWidget {
public:
    explicit TreatmentsPage(DatabaseManager* db, QWidget* parent = nullptr);
private:
    DatabaseManager* m_db{};
};
