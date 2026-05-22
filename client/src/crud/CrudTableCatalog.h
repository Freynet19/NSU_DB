#pragma once

#include <QString>
#include <QVector>

enum class CrudTableGroup
{
    Reference,
    Personnel,
    Production,
    Testing,
    Relations,
};

struct CrudTableDefinition
{
    QString tableName;
    QString displayName;
    QString description;
    CrudTableGroup group = CrudTableGroup::Reference;
};

class CrudTableCatalog
{
public:
    static QVector<CrudTableDefinition> tables();
    static QString groupTitle(CrudTableGroup group);
    static QString tableDisplayName(const QString& tableName);
    static QString tableDescription(const QString& tableName);
    static QString columnTitle(const QString& tableName, const QString& fieldName);
    static bool isReadOnlyColumn(const QString& tableName, const QString& fieldName);
    static bool requiresNonEmptyString(const QString& tableName, const QString& fieldName);
};
