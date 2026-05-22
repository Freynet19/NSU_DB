#include "auth/LoginDialog.h"
#include "db/DatabaseManager.h"
#include "shell/MainWindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("automotive_erp_client"));
    QApplication::setOrganizationName(QStringLiteral("NSU"));

    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QPSQL")))
    {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Ошибка"),
            QObject::tr("Драйвер QPSQL не найден. Установите libqt6-sql6-psql."));
        return 1;
    }

    LoginDialog login;
    if (login.exec() != QDialog::Accepted || !DatabaseManager::instance().isConnected())
    {
        return 0;
    }

    int code = 0;
    {
        MainWindow window(DatabaseManager::instance().role());
        window.show();
        code = app.exec();
    }
    DatabaseManager::instance().disconnect();
    return code;
}
