#pragma once

#include <QDialog>

class QComboBox;
class QLineEdit;
class QSpinBox;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

    QString host() const;
    int port() const;
    QString database() const;
    QString username() const;
    QString password() const;

    void setConnectionError(const QString &message);

private:
    void onLoginAttempt();
    void applyPresetPassword();

    QLineEdit *m_hostEdit = nullptr;
    QSpinBox *m_portSpin = nullptr;
    QLineEdit *m_databaseEdit = nullptr;
    QComboBox *m_userCombo = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
    class QLabel *m_errorLabel = nullptr;
};
