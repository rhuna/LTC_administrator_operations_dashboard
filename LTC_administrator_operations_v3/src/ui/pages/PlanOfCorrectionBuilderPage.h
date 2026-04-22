#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class PlanOfCorrectionBuilderPage : public QWidget {
    Q_OBJECT
public:
    explicit PlanOfCorrectionBuilderPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addItem();
    void markSelectedComplete();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_overdueLabel{nullptr};
    QLabel* m_evidenceLabel{nullptr};
    QLabel* m_completeLabel{nullptr};
    QLineEdit* m_findingDateEdit{nullptr};
    QLineEdit* m_tagEdit{nullptr};
    QLineEdit* m_areaEdit{nullptr};
    QLineEdit* m_findingEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QComboBox* m_severityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_rootCauseEdit{nullptr};
    QTextEdit* m_actionEdit{nullptr};
    QTextEdit* m_evidenceEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_completeButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
