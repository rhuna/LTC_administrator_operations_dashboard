#pragma once
#include <QWidget>
class DatabaseManager;
class CredentialingPage : public QWidget {
public:
    explicit CredentialingPage(DatabaseManager* db, QWidget* parent = nullptr);
};
