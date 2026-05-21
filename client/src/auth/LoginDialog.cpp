#include "auth/LoginDialog.h"

#include "db/DatabaseManager.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Вход в систему"));
    resize(420, 280);

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout;

    m_hostEdit = new QLineEdit(QStringLiteral("localhost"), this);
    m_portSpin = new QSpinBox(this);
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(5432);
    m_databaseEdit = new QLineEdit(QStringLiteral("test"), this);

    m_userCombo = new QComboBox(this);
    m_userCombo->addItem(tr("Администратор (user_admin)"), QStringLiteral("user_admin"));
    m_userCombo->addItem(tr("Кадровик (user_hr)"), QStringLiteral("user_hr"));
    m_userCombo->addItem(tr("Отчётность (user_production)"), QStringLiteral("user_production"));

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    form->addRow(tr("Хост"), m_hostEdit);
    form->addRow(tr("Порт"), m_portSpin);
    form->addRow(tr("База данных"), m_databaseEdit);
    form->addRow(tr("Пользователь"), m_userCombo);
    form->addRow(tr("Пароль"), m_passwordEdit);

    layout->addLayout(form);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setStyleSheet(QStringLiteral("color: #c0392b;"));
    m_errorLabel->hide();
    layout->addWidget(m_errorLabel);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &LoginDialog::onLoginAttempt);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(m_userCombo, &QComboBox::currentIndexChanged, this, [this]() {
        applyPresetPassword();
        m_errorLabel->hide();
    });
    connect(m_passwordEdit, &QLineEdit::textChanged, this, [this]() { m_errorLabel->hide(); });

    m_userCombo->setCurrentIndex(0);
    applyPresetPassword();
}

void LoginDialog::setConnectionError(const QString &message)
{
    if (message.isEmpty()) {
        m_errorLabel->hide();
        m_errorLabel->clear();
        return;
    }

    m_errorLabel->setText(message);
    m_errorLabel->show();
}

void LoginDialog::onLoginAttempt()
{
    m_errorLabel->hide();

    QString error;
    if (!DatabaseManager::instance().connect(host(),
                                             port(),
                                             database(),
                                             username(),
                                             password(),
                                             &error)) {
        setConnectionError(error);
        m_passwordEdit->setFocus();
        m_passwordEdit->selectAll();
        return;
    }

    accept();
}

void LoginDialog::applyPresetPassword()
{
    const QString user = m_userCombo->currentData().toString();
    if (user == QLatin1String("user_admin")) {
        m_passwordEdit->setText(QStringLiteral("admin_password"));
    } else if (user == QLatin1String("user_hr")) {
        m_passwordEdit->setText(QStringLiteral("hr_password"));
    } else {
        m_passwordEdit->setText(QStringLiteral("production_password"));
    }
}

QString LoginDialog::host() const
{
    return m_hostEdit->text().trimmed();
}

int LoginDialog::port() const
{
    return m_portSpin->value();
}

QString LoginDialog::database() const
{
    return m_databaseEdit->text().trimmed();
}

QString LoginDialog::username() const
{
    return m_userCombo->currentData().toString();
}

QString LoginDialog::password() const
{
    return m_passwordEdit->text();
}
