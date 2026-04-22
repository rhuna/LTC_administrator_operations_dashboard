#pragma once

#include <QList>
#include <QPair>
#include <QString>
#include <QWidget>

class DatabaseManager;
class QLabel;
class QListWidget;
class QGridLayout;
class QGroupBox;

class GlobalInsightPanel : public QWidget {
public:
    explicit GlobalInsightPanel(DatabaseManager* db,
                                const QString& contextKey,
                                QWidget* parent = nullptr);

    void refreshData();

private:
    void buildUi();
    void rebuildHighlights(const QList<QPair<QString, QString>>& highlights);
    void setCardText(QLabel* valueLabel, QLabel* hintLabel, const QString& value, const QString& hint);

    DatabaseManager* m_db = nullptr;
    QString m_contextKey;
    QLabel* m_headingLabel = nullptr;
    QLabel* m_summaryLabel = nullptr;
    QListWidget* m_highlightsList = nullptr;
    QListWidget* m_sharedLinksList = nullptr;
    QList<QPair<QLabel*, QLabel*>> m_cards;
};
