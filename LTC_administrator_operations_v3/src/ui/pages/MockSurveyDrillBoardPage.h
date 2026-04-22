#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class MockSurveyDrillBoardPage : public QWidget {
    Q_OBJECT
public:
    explicit MockSurveyDrillBoardPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addDrillItem();
    void markSelectedComplete();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_dueLabel{nullptr};
    QLabel* m_completeLabel{nullptr};
    QLineEdit* m_drillDateEdit{nullptr};
    QLineEdit* m_drillTypeEdit{nullptr};
    QLineEdit* m_areaEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QComboBox* m_priorityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_noteEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_completeButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
