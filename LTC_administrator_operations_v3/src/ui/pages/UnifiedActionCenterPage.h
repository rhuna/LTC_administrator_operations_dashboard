#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class UnifiedActionCenterPage : public QWidget {
    Q_OBJECT
public:
    explicit UnifiedActionCenterPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addManualAction();
    void markSelectedResolved();
    void deleteSelectedManual();

private:
    void populateAutoFeed();
    void populateManualQueue();

    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_criticalLabel{nullptr};
    QLabel* m_dueTodayLabel{nullptr};
    QLabel* m_blockedLabel{nullptr};

    QLineEdit* m_sourceEdit{nullptr};
    QLineEdit* m_itemEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QComboBox* m_severityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_noteEdit{nullptr};

    QPushButton* m_addButton{nullptr};
    QPushButton* m_resolveButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};

    QTableWidget* m_autoFeedTable{nullptr};
    QTableWidget* m_manualQueueTable{nullptr};
};
