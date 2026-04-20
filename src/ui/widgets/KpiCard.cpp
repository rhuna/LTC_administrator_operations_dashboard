#include "KpiCard.h"

#include <QLabel>
#include <QVBoxLayout>

KpiCard::KpiCard(const QString& title, const QString& value, QWidget* parent)
    : QFrame(parent), valueLabel_(new QLabel(value, this)) {
    setObjectName("CardFrame");

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(8);

    auto* titleLabel = new QLabel(title, this);
    titleLabel->setObjectName("CardTitle");

    valueLabel_->setObjectName("CardValue");

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel_);
    layout->addStretch();
}

void KpiCard::setValue(const QString& value) {
    valueLabel_->setText(value);
}
