#include "ResidentsPage.h"

#include <QHeaderView>
#include <QLabel>
#include <QSqlQueryModel>
#include <QTableView>
#include <QVBoxLayout>

#include "data/DatabaseManager.h"

ResidentsPage::ResidentsPage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), db_(db), table_(new QTableView(this)), model_(nullptr) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    auto* title = new QLabel("Residents and Census", this);
    title->setObjectName("PageTitle");
    root->addWidget(title);

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setAlternatingRowColors(true);
    root->addWidget(table_, 1);

    refresh();
}

void ResidentsPage::refresh() {
    delete model_;
    model_ = db_->createResidentsModel(this);
    table_->setModel(model_);
}
