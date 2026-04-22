#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class StaffInterviewPrepBoardPage : public QWidget {
    Q_OBJECT
public:
    explicit StaffInterviewPrepBoardPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addPrepItem();
    void markSelectedReady();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_dueTodayLabel{nullptr};
    QLabel* m_atRiskLabel{nullptr};
    QLabel* m_readyLabel{nullptr};
    QLineEdit* m_interviewDateEdit{nullptr};
    QLineEdit* m_staffNameEdit{nullptr};
    QLineEdit* m_roleEdit{nullptr};
    QLineEdit* m_departmentEdit{nullptr};
    QLineEdit* m_focusAreaEdit{nullptr};
    QLineEdit* m_coachEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QComboBox* m_readinessCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_noteEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_readyButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
