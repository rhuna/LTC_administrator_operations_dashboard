#pragma once

#include <QWidget>

class DatabaseManager;
class QComboBox;
class QLabel;
class QListWidget;
class QTableWidget;

class RoleBasedExecutiveViewsPage : public QWidget {
    Q_OBJECT
public:
    explicit RoleBasedExecutiveViewsPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshView();

private:
    void populatePriorityList(const QString& roleKey);
    void populateBoardSummary(const QString& roleKey);
    void populateFocusList(const QString& roleKey);

    DatabaseManager* m_db{nullptr};
    QComboBox* m_roleCombo{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_primaryCountLabel{nullptr};
    QLabel* m_secondaryCountLabel{nullptr};
    QLabel* m_tertiaryCountLabel{nullptr};
    QListWidget* m_priorityList{nullptr};
    QListWidget* m_focusList{nullptr};
    QTableWidget* m_boardSummaryTable{nullptr};
};
