#pragma once

#include "core/UserRole.h"

#include <QString>

class QSqlDatabase;

class DatabaseManager
{
public:
    static DatabaseManager &instance();

    bool connect(const QString &host,
                 int port,
                 const QString &database,
                 const QString &username,
                 const QString &password,
                 QString *errorMessage = nullptr);

    void disconnect();
    bool isConnected() const;
    QString connectionName() const;
    QString username() const;
    UserRole role() const;

    bool beginTransaction(QString *errorMessage = nullptr);
    bool commit(QString *errorMessage = nullptr);
    bool rollback(QString *errorMessage = nullptr);

    static QString formatConnectionError(const QString &rawError, const QString &username = {});

private:
    DatabaseManager() = default;
    QSqlDatabase connection() const;
    bool loadPrepareStatements(QString *errorMessage);

    QString m_connectionName;
    QString m_username;
    UserRole m_role = UserRole::Unknown;
};
