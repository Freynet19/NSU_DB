#include "db/LookupRepository.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QWidget>

namespace
{
    QString selectSql(LookupKind kind)
    {
        switch (kind)
        {
        case LookupKind::Workshop:
            return QStringLiteral(
                "SELECT workshop_id, name FROM workshop ORDER BY name, workshop_id");
        case LookupKind::Section:
            return QStringLiteral(
                "SELECT s.section_id, s.name || ' (' || w.name || ')' "
                "FROM section s JOIN workshop w USING (workshop_id) "
                "ORDER BY w.name, s.name, s.section_id");
        case LookupKind::ProductCategory:
            return QStringLiteral(
                "SELECT category_id, name FROM product_category ORDER BY name, category_id");
        case LookupKind::Brigade:
            return QStringLiteral(
                "SELECT b.brigade_id, b.name || ' (' || s.name || ')' "
                "FROM brigade b JOIN section s USING (section_id) "
                "ORDER BY s.name, b.name, b.brigade_id");
        case LookupKind::Laboratory:
            return QStringLiteral(
                "SELECT laboratory_id, name FROM laboratory ORDER BY name, laboratory_id");
        case LookupKind::ProductInstance:
            return QStringLiteral(
                "SELECT pi.instance_id, pt.model_name || ' [' || pi.status::text || ']' "
                "FROM product_instance pi "
                "JOIN product_type pt USING (type_id) "
                "ORDER BY pi.instance_id");
        case LookupKind::Employee:
            return QStringLiteral(
                "SELECT e.employee_id, e.full_name "
                "FROM employee e "
                "WHERE COALESCE(("
                "    SELECT pm.movement_type::text "
                "    FROM personnel_movement pm "
                "    WHERE pm.employee_id = e.employee_id "
                "    ORDER BY pm.record_id DESC "
                "    LIMIT 1), '') <> 'dismissal' "
                "ORDER BY e.full_name, e.employee_id");
        case LookupKind::PersonnelCategory:
            return QStringLiteral(
                "SELECT category_code, name || ' (' || type::text || ')' "
                "FROM personnel_category ORDER BY type, category_code");
        case LookupKind::PersonnelCategoryWorker:
            return QStringLiteral(
                "SELECT category_code, name "
                "FROM personnel_category WHERE type = 'Worker' "
                "ORDER BY category_code");
        case LookupKind::PersonnelCategoryItp:
            return QStringLiteral(
                "SELECT category_code, name "
                "FROM personnel_category WHERE type = 'ITP' "
                "ORDER BY category_code");
        case LookupKind::ProductionStage:
            return QStringLiteral(
                "SELECT pc.stage_id, wt.name || ' (этап ' || pc.stage_number || ')' "
                "FROM production_cycle pc "
                "JOIN work_type wt USING (work_type_id) "
                "ORDER BY pc.stage_id");
        case LookupKind::AssemblyRecord:
            return QStringLiteral(
                "SELECT ar.record_id, "
                "'изд. ' || ar.instance_id || ', этап ' || ar.stage_id || ' [' || ar.status::text || ']' "
                "FROM assembly_record ar ORDER BY ar.record_id");
        case LookupKind::Test:
            return QStringLiteral(
                "SELECT test_id, 'изд. ' || instance_id || ' (' || test_date || ')' "
                "FROM test ORDER BY test_id");
        }
        return {};
    }

    QHash<QString, QHash<int, QList<LookupItem>>>& sharedLookupCaches()
    {
        static QHash<QString, QHash<int, QList<LookupItem>>> caches;
        return caches;
    }
} // namespace

QList<LookupItem> loadLookupItems(LookupRepository* lookups, LookupKind kind, QWidget* messageParent)
{
    if (!lookups)
    {
        return {};
    }
    QString err;
    const QList<LookupItem> items = lookups->items(kind, &err);
    if (!err.isEmpty() && messageParent)
    {
        QMessageBox::warning(messageParent,
                             QObject::tr("Справочник"),
                             QObject::tr("Не удалось загрузить справочник: %1").arg(err));
    }
    return items;
}

LookupRepository::LookupRepository(const QString& connectionName)
    : m_connectionName(connectionName)
{
}

void LookupRepository::clearCache()
{
    sharedLookupCaches().remove(m_connectionName);
}

QList<LookupItem> LookupRepository::items(LookupKind kind, QString* errorMessage)
{
    const int key = static_cast<int>(kind);
    QHash<int, QList<LookupItem>>& cache = sharedLookupCaches()[m_connectionName];
    if (cache.contains(key))
    {
        return cache.value(key);
    }

    const QList<LookupItem> loaded = loadItems(kind, errorMessage);
    cache.insert(key, loaded);
    return loaded;
}

QList<LookupItem> LookupRepository::loadItems(LookupKind kind, QString* errorMessage)
{
    QList<LookupItem> result;

    if (m_connectionName.isEmpty() || !QSqlDatabase::contains(m_connectionName))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("Нет подключения к базе данных");
        }
        return result;
    }

    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    const QString sql = selectSql(kind);
    if (!query.exec(sql))
    {
        if (errorMessage)
        {
            *errorMessage = query.lastError().text();
        }
        return result;
    }

    while (query.next())
    {
        LookupItem item;
        item.id = query.value(0).toInt();
        item.label = QStringLiteral("%1 — %2").arg(item.id).arg(query.value(1).toString());
        result.append(item);
    }

    return result;
}
