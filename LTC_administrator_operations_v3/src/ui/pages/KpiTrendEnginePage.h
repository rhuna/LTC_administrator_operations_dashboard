#pragma once

#include <QWidget>

class DatabaseManager;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

class KpiTrendEnginePage : public QWidget {
    Q_OBJECT
public:
    explicit KpiTrendEnginePage(DatabaseManager* db, QWidget* parent = nullptr);

private slots:
    void refreshView();
    void addTrendRow();

private:
    DatabaseManager* m_db{nullptr};
    QLabel* totalRowsLabel{nullptr};
    QLabel* offTrackLabel{nullptr};
    QLabel* watchLabel{nullptr};
    QLabel* measuresLabel{nullptr};

    QLineEdit* metricEdit{nullptr};
    QLineEdit* periodEdit{nullptr};
    QLineEdit* valueEdit{nullptr};
    QLineEdit* targetEdit{nullptr};
    QLineEdit* statusEdit{nullptr};
    QLineEdit* ownerEdit{nullptr};
    QLineEdit* noteEdit{nullptr};
    QTableWidget* tableWidget{nullptr};
};
