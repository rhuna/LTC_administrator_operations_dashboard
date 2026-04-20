#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QSqlQueryModel;
class QTableView;
class QTextEdit;

class IncidentsPage : public QWidget {
    Q_OBJECT
public:
    explicit IncidentsPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddIncidentClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QDateEdit* dateEdit_;
    QLineEdit* residentEdit_;
    QComboBox* severityCombo_;
    QTextEdit* descriptionEdit_;
};
