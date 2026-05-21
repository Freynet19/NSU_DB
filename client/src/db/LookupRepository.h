#pragma once

#include "db/LookupItem.h"

#include <QHash>
#include <QList>
#include <QString>

enum class LookupKind {
    Workshop,
    Section,
    ProductCategory,
    Brigade,
    Laboratory,
    ProductInstance,
    Employee,
    PersonnelCategory,
    ProductionStage,
    AssemblyRecord,
    Test,
};

class LookupRepository
{
public:
    explicit LookupRepository(const QString &connectionName);

    QList<LookupItem> items(LookupKind kind, QString *errorMessage = nullptr);
    void clearCache();

private:
    QList<LookupItem> loadItems(LookupKind kind, QString *errorMessage);

    QString m_connectionName;
    QHash<int, QList<LookupItem>> m_cache;
};
