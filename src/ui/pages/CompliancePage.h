#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QSqlQueryModel;
class QTableView;

class CompliancePage : public QWidget {
    Q_OBJECT
public:
    explicit CompliancePage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddDeadlineClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QLineEdit* areaEdit_;
    QLineEdit* itemEdit_;
    QLineEdit* ownerEdit_;
    QDateEdit* dueDateEdit_;
    QComboBox* statusCombo_;
};
