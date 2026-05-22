#include "crud/CrudTableCatalog.h"

#include <QHash>
#include <QMap>

namespace
{
    struct ColumnMeta
    {
        QString title;
        bool readOnly = false;
        bool requiresNonEmpty = false;
    };

    using ColumnMap = QHash<QString, ColumnMeta>;

    QMap<QString, ColumnMap> buildColumnMaps()
    {
        QMap<QString, ColumnMap> maps;

        auto add = [&](const QString& table,
                       const QString& field,
                       const QString& title,
                       bool readOnly = false,
                       bool requiresNonEmpty = false)
        {
            maps[table].insert(field, ColumnMeta{title, readOnly, requiresNonEmpty});
        };

        add(QStringLiteral("personnel_category"), QStringLiteral("category_code"), QStringLiteral("Код категории"),
            true);
        add(QStringLiteral("personnel_category"), QStringLiteral("name"), QStringLiteral("Наименование"), false,
            true);
        add(QStringLiteral("personnel_category"), QStringLiteral("type"), QStringLiteral("Тип (ITP/Worker)"));

        add(QStringLiteral("product_category"), QStringLiteral("category_id"), QStringLiteral("ID категории"), true);
        add(QStringLiteral("product_category"), QStringLiteral("name"), QStringLiteral("Наименование"), false,
            true);

        add(QStringLiteral("work_type"), QStringLiteral("work_type_id"), QStringLiteral("ID вида работ"), true);
        add(QStringLiteral("work_type"), QStringLiteral("name"), QStringLiteral("Наименование"), false, true);
        add(QStringLiteral("work_type"), QStringLiteral("description"), QStringLiteral("Описание"));

        add(QStringLiteral("laboratory"), QStringLiteral("laboratory_id"), QStringLiteral("ID лаборатории"), true);
        add(QStringLiteral("laboratory"), QStringLiteral("name"), QStringLiteral("Наименование"), false, true);
        add(QStringLiteral("laboratory"), QStringLiteral("location"), QStringLiteral("Расположение"));

        add(QStringLiteral("workshop"), QStringLiteral("workshop_id"), QStringLiteral("ID цеха"), true);
        add(QStringLiteral("workshop"), QStringLiteral("name"), QStringLiteral("Наименование"), false, true);
        add(QStringLiteral("workshop"), QStringLiteral("head_id"), QStringLiteral("ID начальника (ИТП)"));

        add(QStringLiteral("section"), QStringLiteral("section_id"), QStringLiteral("ID участка"), true);
        add(QStringLiteral("section"), QStringLiteral("name"), QStringLiteral("Наименование"), false, true);
        add(QStringLiteral("section"), QStringLiteral("workshop_id"), QStringLiteral("ID цеха"));
        add(QStringLiteral("section"), QStringLiteral("section_head_id"), QStringLiteral("ID начальника участка"));

        add(QStringLiteral("brigade"), QStringLiteral("brigade_id"), QStringLiteral("ID бригады"), true);
        add(QStringLiteral("brigade"), QStringLiteral("name"), QStringLiteral("Наименование"), false, true);
        add(QStringLiteral("brigade"), QStringLiteral("section_id"), QStringLiteral("ID участка"));
        add(QStringLiteral("brigade"), QStringLiteral("foreman_id"), QStringLiteral("ID бригадира"));

        add(QStringLiteral("employee"), QStringLiteral("employee_id"), QStringLiteral("ID сотрудника"), true);
        add(QStringLiteral("employee"), QStringLiteral("full_name"), QStringLiteral("ФИО"), false, true);
        add(QStringLiteral("employee"), QStringLiteral("birth_date"), QStringLiteral("Дата рождения"));
        add(QStringLiteral("employee"), QStringLiteral("hire_date"), QStringLiteral("Дата приёма"));
        add(QStringLiteral("employee"), QStringLiteral("category_code"), QStringLiteral("Код категории"));

        add(QStringLiteral("worker"), QStringLiteral("employee_id"), QStringLiteral("ID сотрудника"), true);
        add(QStringLiteral("worker"), QStringLiteral("specialty"), QStringLiteral("Специальность"), false,
            true);
        add(QStringLiteral("worker"), QStringLiteral("grade"), QStringLiteral("Разряд"));
        add(QStringLiteral("worker"), QStringLiteral("brigade_id"), QStringLiteral("ID бригады"));

        add(QStringLiteral("itp"), QStringLiteral("employee_id"), QStringLiteral("ID сотрудника"), true);
        add(QStringLiteral("itp"), QStringLiteral("position"), QStringLiteral("Должность"), false, true);
        add(QStringLiteral("itp"), QStringLiteral("qualification"), QStringLiteral("Квалификация"));
        add(QStringLiteral("itp"), QStringLiteral("master_section_id"), QStringLiteral("ID участка (мастер)"));

        add(QStringLiteral("product_type"), QStringLiteral("type_id"), QStringLiteral("ID вида изделия"), true);
        add(QStringLiteral("product_type"), QStringLiteral("model_name"), QStringLiteral("Модель"), false,
            true);
        add(QStringLiteral("product_type"), QStringLiteral("category_id"), QStringLiteral("ID категории"));
        add(QStringLiteral("product_type"), QStringLiteral("workshop_id"), QStringLiteral("ID цеха"));
        add(QStringLiteral("product_type"), QStringLiteral("capacity"), QStringLiteral("Вместимость"));
        add(QStringLiteral("product_type"), QStringLiteral("productivity"), QStringLiteral("Производительность"));
        add(QStringLiteral("product_type"), QStringLiteral("load_capacity"), QStringLiteral("Грузоподъёмность"));
        add(QStringLiteral("product_type"), QStringLiteral("engine_type"), QStringLiteral("Тип двигателя"));

        add(QStringLiteral("product_instance"), QStringLiteral("instance_id"), QStringLiteral("ID экземпляра"), true);
        add(QStringLiteral("product_instance"), QStringLiteral("type_id"), QStringLiteral("ID вида изделия"));
        add(QStringLiteral("product_instance"), QStringLiteral("start_date"), QStringLiteral("Дата начала"));
        add(QStringLiteral("product_instance"), QStringLiteral("end_date"), QStringLiteral("Дата окончания"));
        add(QStringLiteral("product_instance"), QStringLiteral("status"), QStringLiteral("Статус"));

        add(QStringLiteral("production_cycle"), QStringLiteral("stage_id"), QStringLiteral("ID этапа"), true);
        add(QStringLiteral("production_cycle"), QStringLiteral("product_type_id"), QStringLiteral("ID вида изделия"));
        add(QStringLiteral("production_cycle"), QStringLiteral("work_type_id"), QStringLiteral("ID вида работ"));
        add(QStringLiteral("production_cycle"), QStringLiteral("section_id"), QStringLiteral("ID участка"));
        add(QStringLiteral("production_cycle"), QStringLiteral("stage_number"), QStringLiteral("Номер этапа"));

        add(QStringLiteral("assembly_record"), QStringLiteral("record_id"), QStringLiteral("ID записи"), true);
        add(QStringLiteral("assembly_record"), QStringLiteral("stage_id"), QStringLiteral("ID этапа"));
        add(QStringLiteral("assembly_record"), QStringLiteral("instance_id"), QStringLiteral("ID экземпляра"));
        add(QStringLiteral("assembly_record"), QStringLiteral("brigade_id"), QStringLiteral("ID бригады"));
        add(QStringLiteral("assembly_record"), QStringLiteral("start_date"), QStringLiteral("Дата начала"));
        add(QStringLiteral("assembly_record"), QStringLiteral("end_date"), QStringLiteral("Дата окончания"));
        add(QStringLiteral("assembly_record"), QStringLiteral("status"), QStringLiteral("Статус"));

        add(QStringLiteral("equipment"), QStringLiteral("equipment_id"), QStringLiteral("ID оборудования"), true);
        add(QStringLiteral("equipment"), QStringLiteral("laboratory_id"), QStringLiteral("ID лаборатории"));
        add(QStringLiteral("equipment"), QStringLiteral("name"), QStringLiteral("Наименование"), false,
            true);
        add(QStringLiteral("equipment"), QStringLiteral("type"), QStringLiteral("Тип"));

        add(QStringLiteral("test"), QStringLiteral("test_id"), QStringLiteral("ID испытания"), true);
        add(QStringLiteral("test"), QStringLiteral("instance_id"), QStringLiteral("ID экземпляра"));
        add(QStringLiteral("test"), QStringLiteral("laboratory_id"), QStringLiteral("ID лаборатории"));
        add(QStringLiteral("test"), QStringLiteral("test_date"), QStringLiteral("Дата испытания"));
        add(QStringLiteral("test"), QStringLiteral("result"), QStringLiteral("Результат"));

        add(QStringLiteral("test_specialist"), QStringLiteral("test_id"), QStringLiteral("ID испытания"));
        add(QStringLiteral("test_specialist"), QStringLiteral("employee_id"), QStringLiteral("ID сотрудника (ИТП)"));

        add(QStringLiteral("test_equipment"), QStringLiteral("test_id"), QStringLiteral("ID испытания"));
        add(QStringLiteral("test_equipment"), QStringLiteral("equipment_id"), QStringLiteral("ID оборудования"));

        add(QStringLiteral("workshop_laboratory"), QStringLiteral("workshop_id"), QStringLiteral("ID цеха"));
        add(QStringLiteral("workshop_laboratory"), QStringLiteral("laboratory_id"), QStringLiteral("ID лаборатории"));

        add(QStringLiteral("personnel_movement"), QStringLiteral("record_id"), QStringLiteral("ID записи"), true);
        add(QStringLiteral("personnel_movement"), QStringLiteral("employee_id"), QStringLiteral("ID сотрудника"));
        add(QStringLiteral("personnel_movement"), QStringLiteral("movement_type"), QStringLiteral("Тип движения"));
        add(QStringLiteral("personnel_movement"), QStringLiteral("date"), QStringLiteral("Дата"));
        add(QStringLiteral("personnel_movement"), QStringLiteral("description"), QStringLiteral("Описание"));

        return maps;
    }

    const QMap<QString, ColumnMap>& columnMaps()
    {
        static const QMap<QString, ColumnMap> maps = buildColumnMaps();
        return maps;
    }

    QVector<CrudTableDefinition> buildTables()
    {
        return {
            {
                QStringLiteral("personnel_category"), QStringLiteral("Категория персонала"),
                QStringLiteral("Справочник категорий ИТП и рабочих."), CrudTableGroup::Reference
            },
            {
                QStringLiteral("product_category"), QStringLiteral("Категория изделий"),
                QStringLiteral("Типы изделий (автобус, троллейбус и т.д.)."), CrudTableGroup::Reference
            },
            {
                QStringLiteral("work_type"), QStringLiteral("Вид работ"),
                QStringLiteral("Виды работ производственного цикла."), CrudTableGroup::Reference
            },
            {
                QStringLiteral("laboratory"), QStringLiteral("Лаборатория"),
                QStringLiteral("Испытательные лаборатории."), CrudTableGroup::Reference
            },

            {
                QStringLiteral("employee"), QStringLiteral("Сотрудник"),
                QStringLiteral("Базовые данные сотрудников."), CrudTableGroup::Personnel
            },
            {
                QStringLiteral("worker"), QStringLiteral("Рабочий"),
                QStringLiteral("Рабочие (подтип сотрудника)."), CrudTableGroup::Personnel
            },
            {
                QStringLiteral("itp"), QStringLiteral("ИТП"),
                QStringLiteral("Инженерно-технический персонал."), CrudTableGroup::Personnel
            },
            {
                QStringLiteral("personnel_movement"), QStringLiteral("Кадровое движение"),
                QStringLiteral("Приём, перевод, увольнение."), CrudTableGroup::Personnel
            },

            {
                QStringLiteral("workshop"), QStringLiteral("Цех"),
                QStringLiteral("Производственные цеха."), CrudTableGroup::Production
            },
            {
                QStringLiteral("section"), QStringLiteral("Участок"),
                QStringLiteral("Участки в составе цеха."), CrudTableGroup::Production
            },
            {
                QStringLiteral("brigade"), QStringLiteral("Бригада"),
                QStringLiteral("Бригады на участках."), CrudTableGroup::Production
            },
            {
                QStringLiteral("product_type"), QStringLiteral("Вид изделия"),
                QStringLiteral("Модели и характеристики изделий."), CrudTableGroup::Production
            },
            {
                QStringLiteral("product_instance"), QStringLiteral("Экземпляр изделия"),
                QStringLiteral("Конкретные экземпляры в производстве."), CrudTableGroup::Production
            },
            {
                QStringLiteral("production_cycle"), QStringLiteral("Производственный цикл"),
                QStringLiteral("Этапы сборки по виду изделия."), CrudTableGroup::Production
            },
            {
                QStringLiteral("assembly_record"), QStringLiteral("Запись сборки"),
                QStringLiteral("Назначение бригад на этапы."), CrudTableGroup::Production
            },

            {
                QStringLiteral("equipment"), QStringLiteral("Оборудование"),
                QStringLiteral("Оборудование лабораторий."), CrudTableGroup::Testing
            },
            {
                QStringLiteral("test"), QStringLiteral("Испытание"),
                QStringLiteral("Испытания экземпляров."), CrudTableGroup::Testing
            },
            {
                QStringLiteral("test_specialist"), QStringLiteral("Испытатель"),
                QStringLiteral("Связь испытания и специалиста ИТП."), CrudTableGroup::Testing
            },
            {
                QStringLiteral("test_equipment"), QStringLiteral("Оборудование испытания"),
                QStringLiteral("Связь испытания и оборудования."), CrudTableGroup::Testing
            },

            {
                QStringLiteral("workshop_laboratory"), QStringLiteral("Лаборатория цеха"),
                QStringLiteral("Привязка лабораторий к цехам."), CrudTableGroup::Relations
            },
        };
    }
} // namespace

QVector<CrudTableDefinition> CrudTableCatalog::tables()
{
    return buildTables();
}

QString CrudTableCatalog::groupTitle(CrudTableGroup group)
{
    switch (group)
    {
    case CrudTableGroup::Reference:
        return QStringLiteral("Справочники");
    case CrudTableGroup::Personnel:
        return QStringLiteral("Персонал");
    case CrudTableGroup::Production:
        return QStringLiteral("Производство");
    case CrudTableGroup::Testing:
        return QStringLiteral("Испытания");
    case CrudTableGroup::Relations:
        return QStringLiteral("Связи");
    }
    return {};
}

QString CrudTableCatalog::tableDisplayName(const QString& tableName)
{
    for (const CrudTableDefinition& def : buildTables())
    {
        if (def.tableName == tableName)
        {
            return def.displayName;
        }
    }
    return tableName;
}

QString CrudTableCatalog::tableDescription(const QString& tableName)
{
    for (const CrudTableDefinition& def : buildTables())
    {
        if (def.tableName == tableName)
        {
            return def.description;
        }
    }
    return {};
}

QString CrudTableCatalog::columnTitle(const QString& tableName, const QString& fieldName)
{
    const ColumnMap& cols = columnMaps().value(tableName);
    const auto it = cols.constFind(fieldName);
    if (it != cols.constEnd())
    {
        return it->title;
    }
    return fieldName;
}

bool CrudTableCatalog::isReadOnlyColumn(const QString& tableName, const QString& fieldName)
{
    const ColumnMap& cols = columnMaps().value(tableName);
    const auto it = cols.constFind(fieldName);
    if (it != cols.constEnd())
    {
        return it->readOnly;
    }
    return false;
}

bool CrudTableCatalog::requiresNonEmptyString(const QString& tableName, const QString& fieldName)
{
    const ColumnMap& cols = columnMaps().value(tableName);
    const auto it = cols.constFind(fieldName);
    if (it != cols.constEnd())
    {
        return it->requiresNonEmpty;
    }
    return false;
}
