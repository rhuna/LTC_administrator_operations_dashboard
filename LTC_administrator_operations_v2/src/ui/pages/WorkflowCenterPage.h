#pragma once
#include <QMap>
#include <QString>
#include <QStringList>
#include <QWidget>

class DatabaseManager;
class QComboBox;
class QFormLayout;
class QLineEdit;
class QPushButton;
class QTableWidget;

class WorkflowCenterPage : public QWidget {
public:
    explicit WorkflowCenterPage(DatabaseManager* db, QWidget* parent = nullptr);

private:
    struct WorkflowDefinition {
        QString tableName;
        QString title;
        QString subtitle;
        QStringList columns;
    };

    void buildEditor();
    void refreshTable();
    void loadSelectedRow();
    void handleSave();
    void handleArchive();
    void handleDelete();
    WorkflowDefinition currentDefinition() const;

    DatabaseManager* m_db{nullptr};
    QComboBox* m_workflowSelector{nullptr};
    QTableWidget* m_table{nullptr};
    QFormLayout* m_editorLayout{nullptr};
    QPushButton* m_saveButton{nullptr};
    QPushButton* m_archiveButton{nullptr};
    QPushButton* m_deleteButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QMap<QString, QLineEdit*> m_editors;
    QList<WorkflowDefinition> m_definitions;
};
