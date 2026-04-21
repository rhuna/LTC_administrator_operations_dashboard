#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QPlainTextEdit;
class QSqlQueryModel;
class QTableView;

class TrainingPage : public QWidget {
    Q_OBJECT
public:
    explicit TrainingPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddTrainingClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QLineEdit* areaEdit_;
    QLineEdit* employeeEdit_;
    QLineEdit* roleEdit_;
    QDateEdit* dueDateEdit_;
    QComboBox* statusCombo_;
    QPlainTextEdit* notesEdit_;
};
