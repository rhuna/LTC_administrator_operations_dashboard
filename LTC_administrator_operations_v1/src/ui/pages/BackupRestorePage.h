#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QListWidget;
class QPushButton;

class BackupRestorePage : public QWidget {
public:
    explicit BackupRestorePage(DatabaseManager* db, QWidget* parent = nullptr);

private:
    void refreshBackupList();
    void refreshSummary();

    DatabaseManager* m_db{nullptr};
    QLabel* m_summaryLabel{nullptr};
    QListWidget* m_backupList{nullptr};
    QPushButton* m_createBackupButton{nullptr};
    QPushButton* m_restoreBackupButton{nullptr};
    QPushButton* m_exportSnapshotButton{nullptr};
    QPushButton* m_openFolderButton{nullptr};
};
