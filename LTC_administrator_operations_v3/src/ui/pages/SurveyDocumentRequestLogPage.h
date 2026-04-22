#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class SurveyDocumentRequestLogPage : public QWidget {
    Q_OBJECT
public:
    explicit SurveyDocumentRequestLogPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addRequest();
    void markSelectedDelivered();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_dueTodayLabel{nullptr};
    QLabel* m_missingLabel{nullptr};
    QLabel* m_deliveredLabel{nullptr};
    QLineEdit* m_requestDateEdit{nullptr};
    QLineEdit* m_requestTimeEdit{nullptr};
    QLineEdit* m_requestSourceEdit{nullptr};
    QLineEdit* m_documentTypeEdit{nullptr};
    QLineEdit* m_documentNameEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_locationEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QComboBox* m_priorityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_noteEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_deliveredButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
