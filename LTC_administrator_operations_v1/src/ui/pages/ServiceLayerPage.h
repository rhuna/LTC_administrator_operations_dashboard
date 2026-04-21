
#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QTableWidget;
class QTextEdit;
class QPushButton;

class ServiceLayerPage : public QWidget {
    Q_OBJECT
public:
    explicit ServiceLayerPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshData();
    void seedDefaultRows();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summary{nullptr};
    QTableWidget* m_table{nullptr};
    QTextEdit* m_notes{nullptr};
    QPushButton* m_seedButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
};
