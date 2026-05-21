#pragma once

#include "core/UserRole.h"

#include <QSqlDatabase>
#include <QString>

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
    QSqlDatabase database() const;
    QString username() const;
    UserRole role() const;

    bool beginTransaction(QString *errorMessage = nullptr);
    bool commit(QString *errorMessage = nullptr);
    bool rollback(QString *errorMessage = nullptr);

    static QString formatConnectionError(const QString &rawError, const QString &username = {});

private:
    DatabaseManager() = default;
    bool loadPrepareStatements(QString *errorMessage);

    QSqlDatabase m_db;
    QString m_username;
    UserRole m_role = UserRole::Unknown;
};
