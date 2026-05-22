#include "db/DatabaseManager.h"

#include <QFile>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

QString DatabaseManager::formatConnectionError(const QString& rawError, const QString& username)
{
    const QString text = rawError.toLower();

    if (text.contains(QStringLiteral("password authentication failed"))
        || text.contains(QStringLiteral("неверный пароль")))
    {
        if (username.isEmpty())
        {
            return QStringLiteral("Неверный пароль.");
        }
        return QStringLiteral("Неверный пароль для пользователя «%1».").arg(username);
    }

    if (text.contains(QStringLiteral("no password supplied")))
    {
        return QStringLiteral("Введите пароль.");
    }

    if (text.contains(QStringLiteral("connection refused"))
        || text.contains(QStringLiteral("could not connect to server")))
    {
        return QStringLiteral(
            "Не удалось подключиться к серверу. Проверьте хост, порт и запуск PostgreSQL (docker compose up -d).");
    }

    if (text.contains(QStringLiteral("timeout")) || text.contains(QStringLiteral("timed out")))
    {
        return QStringLiteral("Истекло время ожидания подключения к серверу.");
    }

    if (text.contains(QStringLiteral("database")) && text.contains(QStringLiteral("does not exist")))
    {
        return QStringLiteral("База данных не найдена. Проверьте имя базы (по умолчанию: test).");
    }

    if (text.contains(QStringLiteral("role")) && text.contains(QStringLiteral("does not exist")))
    {
        return QStringLiteral("Пользователь не найден в базе данных.");
    }

    if (rawError.isEmpty())
    {
        return QStringLiteral("Не удалось подключиться к базе данных.");
    }

    return rawError;
}

QSqlDatabase DatabaseManager::connection() const
{
    if (m_connectionName.isEmpty())
    {
        return QSqlDatabase();
    }
    return QSqlDatabase::database(m_connectionName);
}

bool DatabaseManager::connect(const QString& host,
                              int port,
                              const QString& database,
                              const QString& username,
                              const QString& password,
                              QString* errorMessage)
{
    disconnect();

    const QString connectionName = QStringLiteral("session_%1").arg(QUuid::createUuid().toString());
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), connectionName);
    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(database);
    db.setUserName(username);
    db.setPassword(password);

    if (!db.open())
    {
        if (errorMessage)
        {
            *errorMessage = formatConnectionError(db.lastError().text(), username);
        }
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
        return false;
    }

    m_connectionName = connectionName;
    m_username = username;
    m_role = userRoleFromUsername(username);

    if (!loadPrepareStatements(errorMessage))
    {
        disconnect();
        return false;
    }

    return true;
}

void DatabaseManager::disconnect()
{
    if (m_connectionName.isEmpty())
    {
        return;
    }

    const QString connectionName = m_connectionName;
    m_connectionName.clear();
    m_username.clear();
    m_role = UserRole::Unknown;

    {
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        if (db.isValid() && db.isOpen())
        {
            db.close();
        }
    }

    QSqlDatabase::removeDatabase(connectionName);
}

bool DatabaseManager::isConnected() const
{
    if (m_connectionName.isEmpty() || !QSqlDatabase::contains(m_connectionName))
    {
        return false;
    }
    return QSqlDatabase::database(m_connectionName).isOpen();
}

QString DatabaseManager::connectionName() const
{
    return m_connectionName;
}

QString DatabaseManager::username() const
{
    return m_username;
}

UserRole DatabaseManager::role() const
{
    return m_role;
}

namespace
{
    QString stripLineComments(const QString& sql)
    {
        QStringList lines;
        for (const QString& line : sql.split(QLatin1Char('\n')))
        {
            const QString trimmed = line.trimmed();
            if (trimmed.startsWith(QLatin1String("--")))
            {
                continue;
            }
            lines.append(line);
        }
        return lines.join(QLatin1Char('\n'));
    }
} // namespace

bool DatabaseManager::loadPrepareStatements(QString* errorMessage)
{
    QFile file(QStringLiteral(":/prepare_statements.sql"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("Не удалось открыть prepare_statements.sql");
        }
        return false;
    }

    const QString content = stripLineComments(QString::fromUtf8(file.readAll()));
    const QStringList statements =
        content.split(QRegularExpression(QStringLiteral(";\\s*\\n?")), Qt::SkipEmptyParts);

    QSqlQuery query(connection());
    int preparedCount = 0;
    for (const QString& rawStatement : statements)
    {
        QString statement = rawStatement.trimmed();
        if (statement.isEmpty())
        {
            continue;
        }
        const int preparePos = statement.indexOf(QLatin1String("PREPARE"), 0, Qt::CaseInsensitive);
        if (preparePos < 0)
        {
            continue;
        }
        statement = statement.mid(preparePos);
        if (!query.exec(statement))
        {
            if (errorMessage)
            {
                *errorMessage = QStringLiteral("PREPARE failed: %1\n%2")
                    .arg(query.lastError().text(), statement.left(120));
            }
            return false;
        }
        ++preparedCount;
    }

    if (preparedCount == 0)
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("В prepare_statements.sql не найдено ни одного PREPARE");
        }
        return false;
    }

    return true;
}
