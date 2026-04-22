#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class SurveyEntranceConferenceBoardPage : public QWidget {
    Q_OBJECT
public:
    explicit SurveyEntranceConferenceBoardPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addConferenceItem();
    void markSelectedReady();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_dueLabel{nullptr};
    QLabel* m_readyLabel{nullptr};
    QLineEdit* m_prepDateEdit{nullptr};
    QLineEdit* m_requestEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_locationEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QComboBox* m_priorityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_noteEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_readyButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
