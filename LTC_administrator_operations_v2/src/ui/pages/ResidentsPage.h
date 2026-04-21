#pragma once
#include <QWidget>

class DatabaseManager;
class QLineEdit;
class QPushButton;
class QTableWidget;

class ResidentsPage : public QWidget {
    Q_OBJECT
public:
    explicit ResidentsPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshTable();
    void handleDischargeResident();

private:
    DatabaseManager* m_db{nullptr};
    QTableWidget* m_tableWidget{nullptr};
    QLineEdit* m_roomFilterEdit{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QPushButton* m_dischargeButton{nullptr};
};
