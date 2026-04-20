#pragma once
#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

class StaffingPage : public QWidget {
    Q_OBJECT
public:
    explicit StaffingPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshTables();
    void handleAddAssignment();
    void handleMarkSelectedOpen();
    void handleMarkSelectedFilled();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_snapshotLabel{nullptr};
    QLineEdit* m_dateEdit{nullptr};
    QLineEdit* m_departmentEdit{nullptr};
    QLineEdit* m_shiftEdit{nullptr};
    QLineEdit* m_roleEdit{nullptr};
    QLineEdit* m_employeeEdit{nullptr};
    QLineEdit* m_statusEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_markOpenButton{nullptr};
    QPushButton* m_markFilledButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_assignmentsTable{nullptr};
    QTableWidget* m_minimumsTable{nullptr};
};
