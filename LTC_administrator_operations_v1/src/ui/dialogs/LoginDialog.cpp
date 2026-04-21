#include "LoginDialog.h"

#include "../../data/DatabaseManager.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

LoginDialog::LoginDialog(DatabaseManager* db, QWidget* parent) : QDialog(parent), m_db(db) {
    setWindowTitle("Sign in");
    setModal(true);
    setMinimumWidth(440);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(14);

    auto* card = new QFrame(this);
    card->setObjectName("loginCard");
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 18, 18, 18);
    cardLayout->setSpacing(12);

    auto* title = new QLabel("LTC Administrator Operations Dashboard v26", card);
    title->setStyleSheet("font-size: 22px; font-weight: 700; color: #102a43;");
    auto* subtitle = new QLabel("Role-aware desktop sign-in with demo accounts for administrator, DON, admissions, staffing, and viewer access.", card);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("color: #52606d;");

    auto* form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignLeft);
    form->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    form->setHorizontalSpacing(12);
    form->setVerticalSpacing(10);

    m_userCombo = new QComboBox(card);
    const auto users = m_db ? m_db->fetchUsers() : QList<QMap<QString, QString>>{};
    for (const auto& row : users) {
        const QString display = QString("%1 (%2)").arg(row.value("full_name"), row.value("role_name"));
        m_userCombo->addItem(display, row.value("username"));
    }

    m_passwordEdit = new QLineEdit(card);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("Enter password");

    m_hintLabel = new QLabel("Demo passwords: admin123, don123, admit123, staff123, view123", card);
    m_hintLabel->setWordWrap(true);
    m_hintLabel->setStyleSheet("color: #486581; font-size: 12px;");

    form->addRow("User:", m_userCombo);
    form->addRow("Password:", m_passwordEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, card);
    auto* signInButton = new QPushButton("Sign In", card);
    buttons->addButton(signInButton, QDialogButtonBox::AcceptRole);

    cardLayout->addWidget(title);
    cardLayout->addWidget(subtitle);
    cardLayout->addLayout(form);
    cardLayout->addWidget(m_hintLabel);
    cardLayout->addWidget(buttons);
    root->addWidget(card);

    connect(signInButton, &QPushButton::clicked, this, &LoginDialog::handleLogin);
    connect(buttons, &QDialogButtonBox::rejected, this, &LoginDialog::reject);
    connect(m_userCombo, &QComboBox::currentIndexChanged, this, &LoginDialog::populateSelectedUser);
    populateSelectedUser();
}

QString LoginDialog::fullName() const { return m_fullName; }
QString LoginDialog::role() const { return m_role; }
QString LoginDialog::username() const { return m_username; }

void LoginDialog::populateSelectedUser() {
    const QString username = m_userCombo->currentData().toString();
    if (username == "admin") m_passwordEdit->setPlaceholderText("Hint: admin123");
    else if (username == "don") m_passwordEdit->setPlaceholderText("Hint: don123");
    else if (username == "admissions") m_passwordEdit->setPlaceholderText("Hint: admit123");
    else if (username == "staffing") m_passwordEdit->setPlaceholderText("Hint: staff123");
    else if (username == "viewer") m_passwordEdit->setPlaceholderText("Hint: view123");
}

void LoginDialog::handleLogin() {
    if (!m_db) {
        QMessageBox::warning(this, "Login", "Database manager is not available.");
        return;
    }
    const QString selectedUsername = m_userCombo->currentData().toString();
    QString fullName;
    QString roleName;
    if (!m_db->authenticateUser(selectedUsername, m_passwordEdit->text(), &fullName, &roleName)) {
        QMessageBox::warning(this, "Login failed", "Invalid password for the selected user.");
        return;
    }
    m_username = selectedUsername;
    m_fullName = fullName;
    m_role = roleName;
    accept();
}
