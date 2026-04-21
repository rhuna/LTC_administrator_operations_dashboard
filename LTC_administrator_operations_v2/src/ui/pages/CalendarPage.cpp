#include "CalendarPage.h"
#include "../../data/DatabaseManager.h"

#include <QAbstractItemView>
#include <QCalendarWidget>
#include <QDate>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <algorithm>

CalendarPage::CalendarPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(14);

    auto* heading = new QLabel("Calendar View", this);
    heading->setStyleSheet("font-size: 20px; font-weight: 700;");
    root->addWidget(heading);

    auto* subtitle = new QLabel(
        "See due dates, appointments, admissions, compliance items, MDS dates, and transport activity in one calendar-style operations view.",
        this);
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* summary = new QFrame(this);
    auto* summaryLayout = new QHBoxLayout(summary);
    summaryLayout->setContentsMargins(14, 10, 14, 10);
    summaryLayout->setSpacing(18);

    const int todayCount = db->countWhere("tasks", "due_date='2026-04-20'")
        + db->countWhere("compliance_items", "due_date='2026-04-20'")
        + db->countWhere("transport_items", "appointment_date='2026-04-20'")
        + db->countWhere("admissions", "planned_date='2026-04-20'")
        + db->countWhere("mds_items", "ard_date='2026-04-20' OR triple_check_date='2026-04-20'");
    const int dueSoon = db->dueSoonAlertCount();
    const int overdue = db->overdueAlertCount();

    const QList<QPair<QString, QString>> summaryPairs{{"Today", QString::number(todayCount)}, {"Due soon", QString::number(dueSoon)}, {"Overdue", QString::number(overdue)}};
    for (const auto& pair : summaryPairs) {
        auto* col = new QVBoxLayout();
        auto* label = new QLabel(pair.first, summary);
        label->setStyleSheet("color:#627d98; font-size:12px; font-weight:600;");
        auto* value = new QLabel(pair.second, summary);
        value->setStyleSheet("font-size:24px; font-weight:700; color:#102a43;");
        col->addWidget(label);
        col->addWidget(value);
        summaryLayout->addLayout(col);
    }
    summaryLayout->addStretch();
    root->addWidget(summary);

    auto* content = new QHBoxLayout();
    content->setSpacing(16);

    auto* calendar = new QCalendarWidget(this);
    calendar->setGridVisible(true);
    calendar->setSelectedDate(QDate(2026, 4, 20));
    calendar->setMinimumWidth(420);
    content->addWidget(calendar, 1);

    auto* rightPane = new QVBoxLayout();
    auto* selectedLabel = new QLabel("Items for selected date", this);
    selectedLabel->setStyleSheet("font-size:16px; font-weight:700;");
    rightPane->addWidget(selectedLabel);

    auto* table = new QTableWidget(this);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Module", "Item", "Date", "Owner / Context", "Status"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    rightPane->addWidget(table, 1);

    auto gatherRows = [db](const QString& selected) {
        QList<QMap<QString, QString>> out;
        auto appendRows = [&](const QString& tableName, const QString& dateColumn, const QString& moduleName,
                              const QStringList& columns, const QString& itemColumn, const QString& ownerColumn, const QString& statusColumn) {
            const auto rows = db->fetchTable(tableName, columns);
            for (const auto& row : rows) {
                if (row.value(dateColumn) != selected) continue;
                QMap<QString, QString> mapped;
                mapped["module"] = moduleName;
                mapped["item"] = row.value(itemColumn);
                mapped["date"] = row.value(dateColumn);
                mapped["owner"] = row.value(ownerColumn);
                mapped["status"] = row.value(statusColumn);
                out.append(mapped);
            }
        };

        appendRows("tasks", "due_date", "Tasks", {"due_date", "task_name", "owner", "status"}, "task_name", "owner", "status");
        appendRows("compliance_items", "due_date", "Compliance", {"due_date", "item_name", "owner", "status"}, "item_name", "owner", "status");
        appendRows("credentialing_items", "due_date", "Credentialing", {"due_date", "item_name", "employee_name", "status"}, "item_name", "employee_name", "status");
        appendRows("preparedness_items", "due_date", "Preparedness", {"due_date", "item_name", "owner", "status"}, "item_name", "owner", "status");
        appendRows("transport_items", "appointment_date", "Transportation", {"appointment_date", "appointment_type", "resident_name", "status"}, "appointment_type", "resident_name", "status");
        appendRows("admissions", "planned_date", "Admissions", {"planned_date", "resident_name", "referral_source", "status"}, "resident_name", "referral_source", "status");
        appendRows("huddle_items", "huddle_date", "Huddle", {"huddle_date", "topic", "owner", "status"}, "topic", "owner", "status");
        appendRows("mds_items", "ard_date", "MDS", {"ard_date", "assessment_type", "resident_name", "status"}, "assessment_type", "resident_name", "status");
        appendRows("mds_items", "triple_check_date", "Triple Check", {"triple_check_date", "assessment_type", "resident_name", "status"}, "assessment_type", "resident_name", "status");
        std::sort(out.begin(), out.end(), [](const auto& a, const auto& b) {
            if (a.value("module") == b.value("module")) return a.value("item") < b.value("item");
            return a.value("module") < b.value("module");
        });
        return out;
    };

    auto refresh = [=](const QDate& date) {
        const QString selected = date.toString("yyyy-MM-dd");
        selectedLabel->setText(QString("Items for %1").arg(date.toString("MMMM d, yyyy")));
        const auto rows = gatherRows(selected);
        table->setRowCount(rows.size());
        for (int r = 0; r < rows.size(); ++r) {
            table->setItem(r, 0, new QTableWidgetItem(rows[r].value("module")));
            table->setItem(r, 1, new QTableWidgetItem(rows[r].value("item")));
            table->setItem(r, 2, new QTableWidgetItem(rows[r].value("date")));
            table->setItem(r, 3, new QTableWidgetItem(rows[r].value("owner")));
            table->setItem(r, 4, new QTableWidgetItem(rows[r].value("status")));
        }
    };

    refresh(calendar->selectedDate());
    QObject::connect(calendar, &QCalendarWidget::selectionChanged, this, [=]() {
        refresh(calendar->selectedDate());
    });

    content->addLayout(rightPane, 2);
    root->addLayout(content, 1);
}
