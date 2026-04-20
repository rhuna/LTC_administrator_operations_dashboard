#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QPlainTextEdit;
class QSqlQueryModel;
class QTableView;

class StaffingPage : public QWidget {
    Q_OBJECT
public:
    explicit StaffingPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddStaffingChangeClicked();

private:
    DatabaseManager* db_;
    QTableView* staffingSnapshotTable_;
    QTableView* staffingChangesTable_;
    QSqlQueryModel* staffingSnapshotModel_;
    QSqlQueryModel* staffingChangesModel_;

    QDateEdit* changeDateEdit_;
    QLineEdit* departmentEdit_;
    QComboBox* shiftCombo_;
    QComboBox* changeTypeCombo_;
    QLineEdit* positionEdit_;
    QLineEdit* employeeEdit_;
    QComboBox* impactCombo_;
    QComboBox* statusCombo_;
    QPlainTextEdit* notesEdit_;
};
