#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class BarrierEscalationBoardPage : public QWidget {
    Q_OBJECT
public:
    explicit BarrierEscalationBoardPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addBarrier();
    void markSelectedRemoved();
    void deleteSelected();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_urgentLabel{nullptr};
    QLabel* m_removedLabel{nullptr};
    QLineEdit* m_dateEdit{nullptr};
    QLineEdit* m_departmentEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_barrierEdit{nullptr};
    QLineEdit* m_targetEdit{nullptr};
    QComboBox* m_severityCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_actionEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_removeButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
