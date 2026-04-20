#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QLineEdit;
class QPlainTextEdit;
class QSqlQueryModel;
class QTableView;

class QualityMeasuresPage : public QWidget {
    Q_OBJECT
public:
    explicit QualityMeasuresPage(DatabaseManager* db, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddMeasureClicked();

private:
    DatabaseManager* db_;
    QTableView* table_;
    QSqlQueryModel* model_;
    QLineEdit* measureNameEdit_;
    QLineEdit* ownerEdit_;
    QComboBox* measureSetCombo_;
    QDoubleSpinBox* currentValueSpin_;
    QDoubleSpinBox* targetValueSpin_;
    QDateEdit* reviewDateEdit_;
    QComboBox* statusCombo_;
    QPlainTextEdit* notesEdit_;
};
