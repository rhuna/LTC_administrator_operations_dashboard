#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class LeadershipHuddleGeneratorPage : public QWidget {
    Q_OBJECT
public:
    explicit LeadershipHuddleGeneratorPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addEntry();
    void generateAgenda();
    void markSelectedReady();
    void markSelectedCompleted();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_dueLabel{nullptr};
    QLabel* m_readyLabel{nullptr};
    QLabel* m_completedLabel{nullptr};
    QLineEdit* m_huddleDateEdit{nullptr};
    QLineEdit* m_huddleNameEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_dueTimeEdit{nullptr};
    QComboBox* m_audienceCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_prioritiesEdit{nullptr};
    QTextEdit* m_notesEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_generateButton{nullptr};
    QPushButton* m_readyButton{nullptr};
    QPushButton* m_completedButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
