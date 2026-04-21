#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QPushButton;
class QTableWidget;
class QLineEdit;
class QTextEdit;

class ReleaseCandidatePage : public QWidget {
    Q_OBJECT
public:
    explicit ReleaseCandidatePage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshData();
    void addChecklistItem();
    void seedDefaultRows();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summary{nullptr};
    QTableWidget* m_table{nullptr};
    QPushButton* m_seedButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QLineEdit* m_areaEdit{nullptr};
    QLineEdit* m_itemEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_statusEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QTextEdit* m_notesEdit{nullptr};
};
