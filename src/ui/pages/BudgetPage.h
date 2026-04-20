#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QLineEdit;
class QSqlQueryModel;
class QTableView;

class BudgetPage : public QWidget {
    Q_OBJECT
public:
    explicit BudgetPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddBudgetItemClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QLineEdit* categoryEdit_;
    QLineEdit* ownerEdit_;
    QDoubleSpinBox* budgetSpin_;
    QDoubleSpinBox* actualSpin_;
    QComboBox* statusCombo_;
    QDateEdit* reviewDateEdit_;
};
