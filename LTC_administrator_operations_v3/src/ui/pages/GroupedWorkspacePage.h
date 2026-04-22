#pragma once

#include <QList>
#include <QPair>
#include <QString>
#include <QWidget>

class QLabel;

class DatabaseManager;
class GlobalInsightPanel;
class QTabWidget;

class GroupedWorkspacePage : public QWidget {
public:
    explicit GroupedWorkspacePage(DatabaseManager* db,
                                  const QString& contextKey,
                                  const QString& title,
                                  const QString& subtitle,
                                  const QList<QPair<QString, QWidget*>>& sections,
                                  QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private:
    void refreshConnectedState();
    QString tabLabelFor(const QString& originalTitle) const;

    DatabaseManager* m_db = nullptr;
    QString m_contextKey;
    QList<QPair<QString, QWidget*>> m_sections;
    GlobalInsightPanel* m_summaryPanel = nullptr;
    QTabWidget* m_tabs = nullptr;
    QLabel* m_refreshStatusLabel = nullptr;
};
