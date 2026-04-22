#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class ExecutiveFollowUpBoardPage : public QWidget {
    Q_OBJECT
public:
    explicit ExecutiveFollowUpBoardPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addFollowUp();
    void markSelectedComplete();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};

    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_todayLabel{nullptr};
    QLabel* m_highLabel{nullptr};

    QLineEdit* m_dueDateEdit{nullptr};
    QLineEdit* m_focusAreaEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_sourceEdit{nullptr};
    QComboBox* m_priorityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_itemEdit{nullptr};

    QPushButton* m_addButton{nullptr};
    QPushButton* m_completeButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};

    QTableWidget* m_table{nullptr};
};
