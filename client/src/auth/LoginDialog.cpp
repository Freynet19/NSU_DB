#include "auth/LoginDialog.h"

#include "db/DatabaseManager.h"
#include "ui_LoginDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    resize(420, 280);

    ui->userCombo->addItem(tr("Администратор (user_admin)"), QStringLiteral("user_admin"));
    ui->userCombo->addItem(tr("Кадровик (user_hr)"), QStringLiteral("user_hr"));
    ui->userCombo->addItem(tr("Отчётность (user_production)"), QStringLiteral("user_production"));

    ui->errorLabel->hide();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LoginDialog::onLoginAttempt);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->userCombo, &QComboBox::currentIndexChanged, this, [this]() {
        applyPresetPassword();
        ui->errorLabel->hide();
    });
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, [this]() { ui->errorLabel->hide(); });

    ui->userCombo->setCurrentIndex(0);
    applyPresetPassword();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setConnectionError(const QString &message)
{
    if (message.isEmpty()) {
        ui->errorLabel->hide();
        ui->errorLabel->clear();
        return;
    }

    ui->errorLabel->setText(message);
    ui->errorLabel->show();
}

void LoginDialog::onLoginAttempt()
{
    ui->errorLabel->hide();

    QString error;
    if (!DatabaseManager::instance().connect(host(),
                                             port(),
                                             database(),
                                             username(),
                                             password(),
                                             &error)) {
        setConnectionError(error);
        ui->passwordEdit->setFocus();
        ui->passwordEdit->selectAll();
        return;
    }

    accept();
}

void LoginDialog::applyPresetPassword()
{
    const QString user = ui->userCombo->currentData().toString();
    if (user == QLatin1String("user_admin")) {
        ui->passwordEdit->setText(QStringLiteral("admin_password"));
    } else if (user == QLatin1String("user_hr")) {
        ui->passwordEdit->setText(QStringLiteral("hr_password"));
    } else {
        ui->passwordEdit->setText(QStringLiteral("production_password"));
    }
}

QString LoginDialog::host() const
{
    return ui->hostEdit->text().trimmed();
}

int LoginDialog::port() const
{
    return ui->portSpin->value();
}

QString LoginDialog::database() const
{
    return ui->databaseEdit->text().trimmed();
}

QString LoginDialog::username() const
{
    return ui->userCombo->currentData().toString();
}

QString LoginDialog::password() const
{
    return ui->passwordEdit->text();
}
