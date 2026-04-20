#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QPlainTextEdit;
class QSqlQueryModel;
class QTableView;

class HuddlePage : public QWidget {
    Q_OBJECT
public:
    explicit HuddlePage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddHuddleItemClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;

    QDateEdit* huddleDateEdit_;
    QComboBox* shiftCombo_;
    QLineEdit* departmentEdit_;
    QLineEdit* topicEdit_;
    QLineEdit* ownerEdit_;
    QComboBox* priorityCombo_;
    QComboBox* statusCombo_;
    QPlainTextEdit* notesEdit_;
};
