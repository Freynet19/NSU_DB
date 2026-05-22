#pragma once

#include "db/LookupItem.h"

#include <QList>
#include <QString>

enum class LookupKind
{
    Workshop,
    Section,
    ProductCategory,
    Brigade,
    Laboratory,
    ProductInstance,
    Employee,
    PersonnelCategory,
    PersonnelCategoryWorker,
    PersonnelCategoryItp,
    ProductionStage,
    AssemblyRecord,
    Test,
};

class QWidget;
class LookupRepository;

QList<LookupItem> loadLookupItems(LookupRepository* lookups, LookupKind kind, QWidget* messageParent);

class LookupRepository
{
public:
    explicit LookupRepository(const QString& connectionName);

    QList<LookupItem> items(LookupKind kind, QString* errorMessage = nullptr);
    void clearCache();

private:
    QList<LookupItem> loadItems(LookupKind kind, QString* errorMessage);

    QString m_connectionName;
};
