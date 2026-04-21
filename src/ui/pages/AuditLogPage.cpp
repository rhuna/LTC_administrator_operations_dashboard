#include "AuditLogPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

AuditLogPage::AuditLogPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Audit Log / Change History", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "Review a lightweight local change history for key add, update, admit, discharge, archive, delete, and staffing-status actions across the dashboard.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet("color:#486581; font-weight:600; padding:4px 0 8px 0;");
    root->addWidget(m_summaryLabel);

    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(6);
    m_tableWidget->setHorizontalHeaderLabels({"Timestamp", "Module", "Action", "Item", "Actor", "Details"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    root->addWidget(m_tableWidget, 1);

    const auto rows = m_db->auditLogItems();
    m_summaryLabel->setText(QString("%1 audit event(s) logged locally").arg(rows.size()));

    const QStringList cols{"log_date", "module_name", "action_name", "item_name", "actor_name", "details"};
    m_tableWidget->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < cols.size(); ++c) {
            m_tableWidget->setItem(r, c, new QTableWidgetItem(rows[r].value(cols[c])));
        }
    }
}
