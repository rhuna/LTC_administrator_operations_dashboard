#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QComboBox;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QTextEdit;

class SharedNotesFollowUpPage : public QWidget {
    Q_OBJECT
public:
    explicit SharedNotesFollowUpPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addThread();
    void markSelectedUpdated();
    void markSelectedClosed();
    void deleteSelected();

private:
    void populateSharedLinks();
    void populateThreads();

    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_handoffLabel{nullptr};
    QLabel* m_recentLabel{nullptr};
    QLabel* m_closedLabel{nullptr};

    QComboBox* m_contextCombo{nullptr};
    QLineEdit* m_sourceEdit{nullptr};
    QLineEdit* m_tabsEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QComboBox* m_priorityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_noteEdit{nullptr};
    QTextEdit* m_followupEdit{nullptr};

    QPushButton* m_addButton{nullptr};
    QPushButton* m_updateButton{nullptr};
    QPushButton* m_closeButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};

    QTableWidget* m_sharedLinksTable{nullptr};
    QTableWidget* m_threadsTable{nullptr};
};
