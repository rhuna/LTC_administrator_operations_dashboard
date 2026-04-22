
#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class SurveyRecoveryBoardPage : public QWidget {
    Q_OBJECT
public:
    explicit SurveyRecoveryBoardPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addRecoveryItem();
    void markSelectedComplete();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_overdueLabel{nullptr};
    QLabel* m_completeLabel{nullptr};
    QLineEdit* m_planDateEdit{nullptr};
    QLineEdit* m_focusAreaEdit{nullptr};
    QLineEdit* m_issueEdit{nullptr};
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
