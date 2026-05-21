#include "db/DatabaseManager.h"

#include <QFile>
#include <QRegularExpression>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

bool DatabaseManager::connect(const QString &host,
                              int port,
                              const QString &database,
                              const QString &username,
                              const QString &password,
                              QString *errorMessage)
{
    disconnect();

    const QString connectionName = QStringLiteral("session_%1").arg(QUuid::createUuid().toString());
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), connectionName);
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(database);
    m_db.setUserName(username);
    m_db.setPassword(password);

    if (!m_db.open()) {
        if (errorMessage) {
            *errorMessage = m_db.lastError().text();
        }
        QSqlDatabase::removeDatabase(connectionName);
        m_db = QSqlDatabase();
        return false;
    }

    m_username = username;
    m_role = userRoleFromUsername(username);

    if (!loadPrepareStatements(errorMessage)) {
        disconnect();
        return false;
    }

    return true;
}

void DatabaseManager::disconnect()
{
    if (!m_db.isValid()) {
        return;
    }

    const QString connectionName = m_db.connectionName();
    if (m_db.isOpen()) {
        m_db.close();
    }
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
    m_username.clear();
    m_role = UserRole::Unknown;
}

bool DatabaseManager::isConnected() const
{
    return m_db.isValid() && m_db.isOpen();
}

QSqlDatabase DatabaseManager::database() const
{
    return m_db;
}

QString DatabaseManager::username() const
{
    return m_username;
}

UserRole DatabaseManager::role() const
{
    return m_role;
}

bool DatabaseManager::beginTransaction(QString *errorMessage)
{
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("BEGIN"))) {
        if (errorMessage) {
            *errorMessage = query.lastError().text();
        }
        return false;
    }
    return true;
}

bool DatabaseManager::commit(QString *errorMessage)
{
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("COMMIT"))) {
        if (errorMessage) {
            *errorMessage = query.lastError().text();
        }
        return false;
    }
    return true;
}

bool DatabaseManager::rollback(QString *errorMessage)
{
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("ROLLBACK"))) {
        if (errorMessage) {
            *errorMessage = query.lastError().text();
        }
        return false;
    }
    return true;
}

bool DatabaseManager::loadPrepareStatements(QString *errorMessage)
{
    QFile file(QStringLiteral(":/prepare_statements.sql"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Не удалось открыть prepare_statements.sql");
        }
        return false;
    }

    const QString content = QString::fromUtf8(file.readAll());
    const QStringList statements =
        content.split(QRegularExpression(QStringLiteral(";\\s*\\n?")), Qt::SkipEmptyParts);

    QSqlQuery query(m_db);
    for (const QString &rawStatement : statements) {
        const QString statement = rawStatement.trimmed();
        if (statement.isEmpty() || !statement.startsWith(QLatin1String("PREPARE"), Qt::CaseInsensitive)) {
            continue;
        }
        if (!query.exec(statement)) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("PREPARE failed: %1\n%2")
                                    .arg(query.lastError().text(), statement.left(120));
            }
            return false;
        }
    }

    return true;
}
