#pragma once
#include <QWidget>
class DatabaseManager;
class QualityMeasuresPage : public QWidget {
public:
    explicit QualityMeasuresPage(DatabaseManager* db, QWidget* parent = nullptr);
};
