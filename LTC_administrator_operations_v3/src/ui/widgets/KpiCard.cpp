#include "KpiCard.h"

#include <QLabel>
#include <QVBoxLayout>

KpiCard::KpiCard(const QString& title, const QString& value, QWidget* parent) : QFrame(parent) {
    setObjectName("kpiCard");

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(6);

    auto* t = new QLabel(title, this);
    t->setObjectName("kpiTitle");
    t->setWordWrap(true);

    auto* v = new QLabel(value, this);
    v->setObjectName("kpiValue");

    layout->addWidget(t);
    layout->addWidget(v);
    layout->addStretch();
}
