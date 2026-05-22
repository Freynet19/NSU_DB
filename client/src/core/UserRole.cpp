#include "core/UserRole.h"

QString userRoleDisplayName(UserRole role)
{
    switch (role)
    {
    case UserRole::Admin:
        return QStringLiteral("Администратор БД");
    case UserRole::Hr:
        return QStringLiteral("Кадровик (HR)");
    case UserRole::ProductionReport:
        return QStringLiteral("Производственная отчётность");
    default:
        return QStringLiteral("Неизвестная роль");
    }
}

UserRole userRoleFromUsername(const QString& username)
{
    if (username == QLatin1String("user_admin"))
    {
        return UserRole::Admin;
    }
    if (username == QLatin1String("user_hr"))
    {
        return UserRole::Hr;
    }
    if (username == QLatin1String("user_production"))
    {
        return UserRole::ProductionReport;
    }
    return UserRole::Unknown;
}
