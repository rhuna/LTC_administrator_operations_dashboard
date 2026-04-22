#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QTextEdit;

class ExecutivePrintExportCenterPage : public QWidget {
    Q_OBJECT
public:
    explicit ExecutivePrintExportCenterPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshBoard();
    void addPacket();
    void markSelectedReady();
    void markSelectedExported();
    void deleteSelected();
    void exportSnapshot();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QLabel* m_openLabel{nullptr};
    QLabel* m_dueLabel{nullptr};
    QLabel* m_readyLabel{nullptr};
    QLabel* m_exportedLabel{nullptr};
    QLineEdit* m_packDateEdit{nullptr};
    QLineEdit* m_packetNameEdit{nullptr};
    QLineEdit* m_audienceEdit{nullptr};
    QLineEdit* m_scopeEdit{nullptr};
    QLineEdit* m_ownerEdit{nullptr};
    QLineEdit* m_dueDateEdit{nullptr};
    QComboBox* m_formatCombo{nullptr};
    QComboBox* m_statusCombo{nullptr};
    QTextEdit* m_noteEdit{nullptr};
    QPushButton* m_addButton{nullptr};
    QPushButton* m_readyButton{nullptr};
    QPushButton* m_exportedButton{nullptr};
    QPushButton* m_exportSnapshotButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QTableWidget* m_table{nullptr};
};
