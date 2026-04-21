#pragma once
#include <QWidget>

class DatabaseManager;
class QLineEdit;
class QPushButton;
class QTableWidget;

class CensusManagementPage : public QWidget {
    Q_OBJECT
public:
    explicit CensusManagementPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshTable();
    void handleAddEvent();

private:
    DatabaseManager* m_db{nullptr};
    QLineEdit* m_eventDateEdit{nullptr};
    QLineEdit* m_residentNameEdit{nullptr};
    QLineEdit* m_eventTypeEdit{nullptr};
    QLineEdit* m_roomEdit{nullptr};
    QLineEdit* m_payerEdit{nullptr};
    QLineEdit* m_statusEdit{nullptr};
    QLineEdit* m_notesEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_tableWidget{nullptr};
};
