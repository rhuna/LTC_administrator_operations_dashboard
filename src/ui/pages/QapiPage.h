#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QPlainTextEdit;
class QSqlQueryModel;
class QTableView;

class QapiPage : public QWidget {
    Q_OBJECT
public:
    explicit QapiPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddProjectClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QLineEdit* topicEdit_;
    QLineEdit* ownerEdit_;
    QComboBox* sourceCombo_;
    QComboBox* stageCombo_;
    QDateEdit* reviewDateEdit_;
    QPlainTextEdit* aimEdit_;
};
