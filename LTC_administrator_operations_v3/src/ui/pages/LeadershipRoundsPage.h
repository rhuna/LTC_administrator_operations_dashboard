#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QTextEdit;

class LeadershipRoundsPage : public QWidget {
    Q_OBJECT
public:
    explicit LeadershipRoundsPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshView();
    void addRoundNote();

private:
    DatabaseManager* m_db{nullptr};

    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openFollowupLabel{nullptr};
    QLabel* m_highPriorityLabel{nullptr};
    QLabel* m_completedLabel{nullptr};

    QLineEdit* m_roundDateEdit{nullptr};
    QLineEdit* m_shiftEdit{nullptr};
    QLineEdit* m_areaEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_priorityEdit{nullptr};
    QLineEdit* m_statusEdit{nullptr};
    QLineEdit* m_followupDateEdit{nullptr};
    QTextEdit* m_noteEdit{nullptr};

    QPushButton* m_addButton{nullptr};
    QPushButton* m_refreshButton{nullptr};

    QTableWidget* m_table{nullptr};
};
