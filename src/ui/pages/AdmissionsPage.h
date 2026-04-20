#pragma once
#include <QWidget>

class DatabaseManager;
class QLineEdit;
class QPushButton;
class QTableWidget;

class AdmissionsPage : public QWidget {
    Q_OBJECT
public:
    explicit AdmissionsPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshTable();
    void handleAddReferral();
    void handleAdmitSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLineEdit* m_nameEdit{nullptr};
    QLineEdit* m_sourceEdit{nullptr};
    QLineEdit* m_dateEdit{nullptr};
    QLineEdit* m_statusEdit{nullptr};
    QLineEdit* m_roomEdit{nullptr};
    QLineEdit* m_payerEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_admitButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_tableWidget{nullptr};
};
