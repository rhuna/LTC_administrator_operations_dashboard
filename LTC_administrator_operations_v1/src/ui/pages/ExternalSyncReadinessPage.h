#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QPushButton;
class QTableWidget;
class QLineEdit;
class QTextEdit;

class ExternalSyncReadinessPage : public QWidget {
    Q_OBJECT
public:
    explicit ExternalSyncReadinessPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshData();
    void addProfile();
    void seedDefaultRows();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summary{nullptr};
    QTableWidget* m_table{nullptr};
    QPushButton* m_seedButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QLineEdit* m_systemNameEdit{nullptr};
    QLineEdit* m_entityTypeEdit{nullptr};
    QLineEdit* m_directionEdit{nullptr};
    QLineEdit* m_statusEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QTextEdit* m_notesEdit{nullptr};
};
