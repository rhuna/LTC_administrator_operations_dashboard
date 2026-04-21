#pragma once

#include <QDialog>

class DatabaseManager;
class QComboBox;
class QLineEdit;
class QLabel;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(DatabaseManager* db, QWidget* parent = nullptr);

    QString fullName() const;
    QString role() const;
    QString username() const;

private slots:
    void handleLogin();
    void populateSelectedUser();

private:
    DatabaseManager* m_db{nullptr};
    QComboBox* m_userCombo{nullptr};
    QLineEdit* m_passwordEdit{nullptr};
    QLabel* m_hintLabel{nullptr};
    QString m_fullName;
    QString m_role;
    QString m_username;
};
