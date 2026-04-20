#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QSqlQueryModel;
class QTableView;

class AdmissionsPage : public QWidget {
    Q_OBJECT
public:
    explicit AdmissionsPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddAdmissionClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QLineEdit* residentEdit_;
    QLineEdit* referralEdit_;
    QLineEdit* payerEdit_;
    QDateEdit* expectedDateEdit_;
    QComboBox* statusCombo_;
};
