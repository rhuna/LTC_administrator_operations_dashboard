#include "BackupRestorePage.h"
#include "../../data/DatabaseManager.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

BackupRestorePage::BackupRestorePage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Backup / Restore / Data Management", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Create timestamped local backups, export a lightweight operational snapshot, and restore from a selected backup when you need a safer rollback path.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* actionsBox = new QGroupBox("Quick data protection actions", this);
    auto* actionsLayout = new QVBoxLayout(actionsBox);

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setWordWrap(true);
    actionsLayout->addWidget(m_summaryLabel);

    auto* buttonRow = new QHBoxLayout();
    m_createBackupButton = new QPushButton("Create Backup", this);
    m_restoreBackupButton = new QPushButton("Restore Selected Backup", this);
    m_exportSnapshotButton = new QPushButton("Export JSON Snapshot", this);
    m_openFolderButton = new QPushButton("Open Backup Folder", this);
    buttonRow->addWidget(m_createBackupButton);
    buttonRow->addWidget(m_restoreBackupButton);
    buttonRow->addWidget(m_exportSnapshotButton);
    buttonRow->addWidget(m_openFolderButton);
    buttonRow->addStretch(1);
    actionsLayout->addLayout(buttonRow);

    root->addWidget(actionsBox);

    auto* listBox = new QGroupBox("Available backups", this);
    auto* listLayout = new QVBoxLayout(listBox);
    m_backupList = new QListWidget(this);
    listLayout->addWidget(m_backupList);
    root->addWidget(listBox, 1);

    connect(m_createBackupButton, &QPushButton::clicked, this, [this]() {
        QString createdPath;
        if (m_db->createBackupCopy(&createdPath)) {
            QMessageBox::information(
                this,
                "Backup created",
                QString("Created backup:\n%1").arg(createdPath));
            refreshBackupList();
            refreshSummary();
        } else {
            QMessageBox::warning(this, "Backup failed", "Could not create a backup copy of the current local database.");
        }
    });

    connect(m_restoreBackupButton, &QPushButton::clicked, this, [this]() {
        const auto* item = m_backupList->currentItem();
        if (!item) {
            QMessageBox::information(this, "Select a backup", "Choose a backup file from the list first.");
            return;
        }
        const QString path = item->data(Qt::UserRole).toString();
        const auto reply = QMessageBox::question(
            this,
            "Restore backup",
            QString("Restore this backup?\n\n%1\n\nThe app will reconnect to the restored database file.").arg(path));
        if (reply != QMessageBox::Yes) {
            return;
        }

        if (m_db->restoreFromBackup(path)) {
            QMessageBox::information(this, "Restore complete", "The selected backup was restored successfully. Refresh pages as needed.");
            refreshBackupList();
            refreshSummary();
        } else {
            QMessageBox::warning(this, "Restore failed", "Could not restore the selected backup.");
        }
    });

    connect(m_exportSnapshotButton, &QPushButton::clicked, this, [this]() {
        QString exportPath;
        if (m_db->exportJsonSnapshot(&exportPath)) {
            QMessageBox::information(
                this,
                "Snapshot exported",
                QString("Exported JSON snapshot:\n%1").arg(exportPath));
        } else {
            QMessageBox::warning(this, "Export failed", "Could not export the JSON data snapshot.");
        }
    });

    connect(m_openFolderButton, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_db->backupDirectoryPath()));
    });

    refreshBackupList();
    refreshSummary();
}

void BackupRestorePage::refreshBackupList() {
    m_backupList->clear();
    const auto backups = m_db->availableBackupFiles();
    for (const auto& path : backups) {
        const QString label = QString("%1 — %2").arg(QFileInfo(path).fileName(), path);
        auto* item = new QListWidgetItem(label, m_backupList);
        item->setData(Qt::UserRole, path);
    }
}

void BackupRestorePage::refreshSummary() {
    const auto backups = m_db->availableBackupFiles();
    m_summaryLabel->setText(
        QString(
            "Current local database: %1\n"
            "Backup folder: %2\n"
            "Available backups: %3\n\n"
            "Use JSON export when you want a lightweight snapshot of operational records that can be reviewed outside the app.")
            .arg(m_db->databaseFilePath(), m_db->backupDirectoryPath(), QString::number(backups.size())));
}
