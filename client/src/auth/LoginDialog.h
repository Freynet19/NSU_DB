#pragma once

#include <QDialog>

namespace Ui
{
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog() override;

    QString host() const;
    int port() const;
    QString database() const;
    QString username() const;
    QString password() const;

    void setConnectionError(const QString& message);

private:
    void onLoginAttempt();
    void applyPresetPassword();

    Ui::LoginDialog* ui = nullptr;
};
