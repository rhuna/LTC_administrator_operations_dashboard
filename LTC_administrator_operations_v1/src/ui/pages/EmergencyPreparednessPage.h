#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QPlainTextEdit;
class QSqlQueryModel;
class QTableView;

class EmergencyPreparednessPage : public QWidget {
    Q_OBJECT
public:
    explicit EmergencyPreparednessPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddPreparednessItemClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QDateEdit* drillDateEdit_;
    QLineEdit* scenarioEdit_;
    QLineEdit* ownerEdit_;
    QDateEdit* nextDrillEdit_;
    QComboBox* statusCombo_;
    QPlainTextEdit* notesEdit_;
};
