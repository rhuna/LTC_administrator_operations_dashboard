#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class ResidentTracerManagerPage : public QWidget {
    Q_OBJECT
public:
    explicit ResidentTracerManagerPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addTracer();
    void markSelectedResolved();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_activeLabel{nullptr};
    QLabel* m_highRiskLabel{nullptr};
    QLabel* m_followupLabel{nullptr};
    QLabel* m_resolvedLabel{nullptr};
    QLineEdit* m_tracerDateEdit{nullptr};
    QLineEdit* m_residentEdit{nullptr};
    QLineEdit* m_roomEdit{nullptr};
    QLineEdit* m_tracerTypeEdit{nullptr};
    QLineEdit* m_focusAreaEdit{nullptr};
    QLineEdit* m_departmentEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QComboBox* m_riskCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_noteEdit{nullptr};
    QTextEdit* m_followupEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_resolvedButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
