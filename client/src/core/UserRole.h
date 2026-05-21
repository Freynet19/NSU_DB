#pragma once

#include <QString>

enum class UserRole {
    Unknown,
    Admin,
    Hr,
    ProductionReport,
};

QString userRoleDisplayName(UserRole role);
UserRole userRoleFromUsername(const QString &username);
