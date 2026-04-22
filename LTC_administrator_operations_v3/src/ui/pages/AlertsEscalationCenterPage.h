#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class AlertsEscalationCenterPage : public QWidget {
    Q_OBJECT
public:
    explicit AlertsEscalationCenterPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addEscalation();
    void markSelectedResolved();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_totalLabel{nullptr};
    QLabel* m_criticalLabel{nullptr};
    QLabel* m_overdueLabel{nullptr};
    QLabel* m_blockedLabel{nullptr};
    QLineEdit* m_alertDateEdit{nullptr};
    QLineEdit* m_boardEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QLineEdit* m_itemEdit{nullptr};
    QComboBox* m_severityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_noteEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_resolveButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
