#include "auth/LoginDialog.h"
#include "db/DatabaseManager.h"
#include "shell/MainWindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("automotive_erp_client"));
    QApplication::setOrganizationName(QStringLiteral("NSU"));

    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QPSQL"))) {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Ошибка"),
            QObject::tr("Драйвер QPSQL не найден. Установите libqt6-sql6-psql."));
        return 1;
    }

    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }

    QString error;
    if (!DatabaseManager::instance().connect(login.host(),
                                             login.port(),
                                             login.database(),
                                             login.username(),
                                             login.password(),
                                             &error)) {
        QMessageBox::critical(nullptr, QObject::tr("Подключение"), error);
        return 1;
    }

    MainWindow window(DatabaseManager::instance().role());
    window.show();

    const int code = app.exec();
    DatabaseManager::instance().disconnect();
    return code;
}
