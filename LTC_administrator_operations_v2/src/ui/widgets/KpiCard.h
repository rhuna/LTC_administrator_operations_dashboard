#pragma once
#include <QFrame>
class QLabel;
class KpiCard : public QFrame {
public:
    KpiCard(const QString& title, const QString& value, QWidget* parent = nullptr);
};
