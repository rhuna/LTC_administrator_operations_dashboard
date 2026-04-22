#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class MorningMeetingBoardPage : public QWidget {
    Q_OBJECT
public:
    explicit MorningMeetingBoardPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addPriorityItem();
    void markSelectedDone();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_activeLabel{nullptr};
    QLabel* m_todayLabel{nullptr};
    QLabel* m_blockedLabel{nullptr};
    QLineEdit* m_dateEdit{nullptr};
    QLineEdit* m_departmentEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_dueTimeEdit{nullptr};
    QComboBox* m_priorityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_itemEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_doneButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
