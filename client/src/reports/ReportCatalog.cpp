#include "reports/ReportCatalog.h"

#include <QObject>

namespace {

const ReportDefinition kAllReports[] = {
    {ReportId::ProductTypes,
     QStringLiteral("Виды изделий"),
     QObject::tr("Получить перечень видов изделий отдельной категории и в целом, "
                 "собираемых указанным цехом, предприятием.")},
    {ReportId::FinishedInPeriod,
     QStringLiteral("Собранные за период"),
     QObject::tr("Получить перечень изделий отдельной категории и в целом, собранных "
                 "указанным цехом, участком, предприятием за определённый отрезок времени.")},
    {ReportId::Personnel,
     QStringLiteral("Кадровый состав"),
     QObject::tr("Получить данные о кадровом составе цеха, предприятия в целом и по "
                 "указанным категориям инженерно-технического персонала и рабочих.")},
    {ReportId::Sections,
     QStringLiteral("Участки и начальники"),
     QObject::tr("Получить перечень участков указанного цеха, предприятия в целом и их "
                 "начальников.")},
    {ReportId::ProductWorks,
     QStringLiteral("Работы изделия"),
     QObject::tr("Получить перечень работ, которые проходит указанное изделие.")},
    {ReportId::BrigadeComposition,
     QStringLiteral("Состав бригад"),
     QObject::tr("Получить состав бригад указанного участка, цеха.")},
    {ReportId::SectionMasters,
     QStringLiteral("Мастера участков"),
     QObject::tr("Получить перечень мастеров указанного участка, цеха.")},
    {ReportId::CurrentAssembling,
     QStringLiteral("Собираемые сейчас"),
     QObject::tr("Получить перечень изделий отдельной категории и в целом, собираемых "
                 "в настоящий момент указанным участком, цехом, предприятием.")},
    {ReportId::ProductBrigades,
     QStringLiteral("Бригады сборки изделия"),
     QObject::tr("Получить состав бригад, участвующих в сборке указанного изделия.")},
    {ReportId::ProductLaboratories,
     QStringLiteral("Лаборатории изделия"),
     QObject::tr("Получить перечень испытательных лабораторий, участвующих в испытаниях "
                 "конкретного изделия.")},
    {ReportId::TestedInLab,
     QStringLiteral("Испытанные в лаборатории"),
     QObject::tr("Получить перечень изделий отдельной категории и в целом, проходивших "
                 "испытание в указанной лаборатории за определённый период.")},
    {ReportId::TestSpecialists,
     QStringLiteral("Испытатели"),
     QObject::tr("Получить перечень испытателей, участвующих в испытаниях указанного "
                 "изделия, изделий категории и в целом в лаборатории за период.")},
    {ReportId::TestEquipment,
     QStringLiteral("Оборудование испытаний"),
     QObject::tr("Получить состав оборудования, использовавшегося при испытании "
                 "изделия, изделий категории и в целом в лаборатории за период.")},
    {ReportId::CurrentList,
     QStringLiteral("Собираемые (список)"),
     QObject::tr("Получить перечень изделий отдельной категории и в целом, собираемых "
                 "указанным цехом, участком, предприятием в настоящее время.")},
};

int reportIndex(ReportId id)
{
    return static_cast<int>(id) - 1;
}

bool roleHasReport(UserRole role, ReportId id)
{
    switch (role) {
    case UserRole::Admin:
        return true;
    case UserRole::Hr:
        return id == ReportId::Personnel || id == ReportId::Sections
               || id == ReportId::BrigadeComposition || id == ReportId::SectionMasters;
    case UserRole::ProductionReport:
        return id == ReportId::ProductTypes || id == ReportId::FinishedInPeriod
               || id == ReportId::CurrentAssembling || id == ReportId::ProductLaboratories
               || id == ReportId::TestedInLab || id == ReportId::CurrentList;
    default:
        return false;
    }
}

} // namespace

const ReportDefinition &ReportCatalog::definition(ReportId id)
{
    return kAllReports[reportIndex(id)];
}

QList<ReportDefinition> ReportCatalog::reportsForRole(UserRole role)
{
    QList<ReportDefinition> result;
    for (const ReportDefinition &report : kAllReports) {
        if (roleHasReport(role, report.id)) {
            result.append(report);
        }
    }
    return result;
}

QString ReportCatalog::listItemTitle(const ReportDefinition &report)
{
    return QStringLiteral("%1. %2").arg(static_cast<int>(report.id)).arg(report.shortTitle);
}
