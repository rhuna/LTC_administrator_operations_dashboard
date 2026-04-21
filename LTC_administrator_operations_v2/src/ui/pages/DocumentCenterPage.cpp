#include "DocumentCenterPage.h"
#include "../../data/DatabaseManager.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QStandardPaths>
#include <QTableWidget>
#include <QVBoxLayout>

DocumentCenterPage::DocumentCenterPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Document Center", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Import actual files into the dashboard's local document store, including referral packets from the waitlist workflow, and track which operational item they belong to.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* tableWidget = new QTableWidget(this);
    const QStringList cols{
        "module_name", "document_name", "document_type", "linked_item", "owner", "status", "file_path", "imported_on", "notes"
    };
    tableWidget->setColumnCount(cols.size());
    tableWidget->setHorizontalHeaderLabels({"Module", "Document", "Type", "Linked Item", "Owner", "Status", "Stored File", "Imported", "Notes"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto refreshTable = [db, tableWidget, cols]() {
        tableWidget->setRowCount(0);
        const auto rows = db->fetchTable("document_items", cols);
        for (const auto& row : rows) {
            const int r = tableWidget->rowCount();
            tableWidget->insertRow(r);
            for (int c = 0; c < cols.size(); ++c) {
                tableWidget->setItem(r, c, new QTableWidgetItem(row.value(cols[c])));
            }
        }
    };

    auto* form = new QHBoxLayout();
    auto* moduleName = new QLineEdit(this);
    moduleName->setPlaceholderText("Module");
    auto* documentName = new QLineEdit(this);
    documentName->setPlaceholderText("Document name");
    auto* documentType = new QLineEdit(this);
    documentType->setPlaceholderText("Type");
    auto* linkedItem = new QLineEdit(this);
    linkedItem->setPlaceholderText("Linked item");
    auto* owner = new QLineEdit(this);
    owner->setPlaceholderText("Owner");
    auto* filePath = new QLineEdit(this);
    filePath->setPlaceholderText("Choose a file to import");
    auto* notes = new QLineEdit(this);
    notes->setPlaceholderText("Notes");
    auto* browseButton = new QPushButton("Browse", this);
    auto* importButton = new QPushButton("Import Document", this);

    form->addWidget(moduleName);
    form->addWidget(documentName);
    form->addWidget(documentType);
    form->addWidget(linkedItem);
    form->addWidget(owner);
    form->addWidget(filePath);
    form->addWidget(browseButton);
    form->addWidget(notes);
    form->addWidget(importButton);

    root->addLayout(form);
    root->addWidget(tableWidget, 1);

    QObject::connect(browseButton, &QPushButton::clicked, this, [=]() {
        const QString chosen = QFileDialog::getOpenFileName(this, "Select document to import");
        if (!chosen.isEmpty()) {
            filePath->setText(chosen);
            if (documentName->text().trimmed().isEmpty()) {
                documentName->setText(QFileInfo(chosen).fileName());
            }
        }
    });

    QObject::connect(importButton, &QPushButton::clicked, this, [=]() {
        QString storedPath = filePath->text().trimmed();
        if (!storedPath.isEmpty()) {
            const QString storageRoot = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("documents");
            QDir().mkpath(storageRoot);
            const QFileInfo info(storedPath);
            const QString targetPath = QDir(storageRoot).filePath(info.fileName());
            if (QFile::exists(targetPath)) {
                QFile::remove(targetPath);
            }
            QFile::copy(storedPath, targetPath);
            storedPath = targetPath;
        }

        db->addRecord("document_items", {
            {"module_name", moduleName->text()},
            {"document_name", documentName->text().isEmpty() ? QString("Imported document") : documentName->text()},
            {"document_type", documentType->text().isEmpty() ? QString("Document") : documentType->text()},
            {"linked_item", linkedItem->text()},
            {"owner", owner->text()},
            {"status", "Open"},
            {"file_path", storedPath},
            {"imported_on", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm")},
            {"notes", notes->text()}
        });
        refreshTable();
        moduleName->clear();
        documentName->clear();
        documentType->clear();
        linkedItem->clear();
        owner->clear();
        filePath->clear();
        notes->clear();
    });

    refreshTable();
}
