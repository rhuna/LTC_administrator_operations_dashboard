#pragma once

#include <QFrame>

class QLabel;

class KpiCard : public QFrame {
    Q_OBJECT
public:
    explicit KpiCard(const QString& title, const QString& value, QWidget* parent = nullptr);
    void setValue(const QString& value);

private:
    QLabel* valueLabel_;
};
