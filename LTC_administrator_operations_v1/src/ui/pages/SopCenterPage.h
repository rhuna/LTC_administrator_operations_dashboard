#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QPushButton;
class QTableWidget;
class QLineEdit;
class QTextEdit;

class SopCenterPage : public QWidget {
    Q_OBJECT
public:
    explicit SopCenterPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshData();
    void addSopItem();
    void seedDefaultRows();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summary{nullptr};
    QTableWidget* m_table{nullptr};
    QPushButton* m_seedButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QLineEdit* m_areaEdit{nullptr};
    QLineEdit* m_titleEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_statusEdit{nullptr};
    QLineEdit* m_lastReviewedEdit{nullptr};
    QTextEdit* m_notesEdit{nullptr};
};
