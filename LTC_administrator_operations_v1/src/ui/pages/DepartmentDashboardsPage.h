#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QTableWidget;

class DepartmentDashboardsPage : public QWidget {
    Q_OBJECT
public:
    explicit DepartmentDashboardsPage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshData();

private:
    void populateAdmissionsTable();
    void populateSupportTable();

    DatabaseManager* m_db {nullptr};

    QLabel* nursingSummaryLabel {nullptr};
    QLabel* admissionsSummaryLabel {nullptr};
    QLabel* reimbursementSummaryLabel {nullptr};
    QLabel* supportSummaryLabel {nullptr};

    QTableWidget* admissionsTable {nullptr};
    QTableWidget* supportTable {nullptr};
};
