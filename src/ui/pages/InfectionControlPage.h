#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QPlainTextEdit;
class QSqlQueryModel;
class QTableView;

class InfectionControlPage : public QWidget {
    Q_OBJECT
public:
    explicit InfectionControlPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddInfectionItemClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QDateEdit* eventDateEdit_;
    QLineEdit* areaEdit_;
    QLineEdit* ownerEdit_;
    QDateEdit* nextReviewEdit_;
    QComboBox* statusCombo_;
    QPlainTextEdit* notesEdit_;
};
