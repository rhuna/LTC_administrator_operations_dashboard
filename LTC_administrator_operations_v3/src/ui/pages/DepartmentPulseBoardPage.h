#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class DepartmentPulseBoardPage : public QWidget {
    Q_OBJECT
public:
    explicit DepartmentPulseBoardPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addPulseItem();
    void markSelectedResolved();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_activeLabel{nullptr};
    QLabel* m_elevatedLabel{nullptr};
    QLabel* m_blockedLabel{nullptr};
    QLineEdit* m_dateEdit{nullptr};
    QLineEdit* m_departmentEdit{nullptr};
    QLineEdit* m_leaderEdit{nullptr};
    QComboBox* m_riskCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QLineEdit* m_stateEdit{nullptr};
    QTextEdit* m_blockerEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_resolveButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
