#include "procedures/ProcedureCatalog.h"

#include <QObject>

namespace
{
    const ProcedureDefinition kAllProcedures[] = {
        {
            ProcedureId::HireWorker,
            QStringLiteral("Приём рабочего"),
            QObject::tr("Регистрация нового рабочего: ФИО, даты, категория, специальность, разряд, "
                "опционально бригада."),
            QStringLiteral("proc_hire_worker")
        },
        {
            ProcedureId::HireItp,
            QStringLiteral("Приём ИТП"),
            QObject::tr("Регистрация инженерно-технического работника: ФИО, даты, категория, "
                "должность, квалификация."),
            QStringLiteral("proc_hire_itp")
        },
        {
            ProcedureId::TransferEmployee,
            QStringLiteral("Перевод сотрудника"),
            QObject::tr("Перевод сотрудника в другую бригаду и/или на участок мастера с записью в "
                "кадровое движение."),
            QStringLiteral("proc_transfer_employee")
        },
        {
            ProcedureId::DismissEmployee,
            QStringLiteral("Увольнение"),
            QObject::tr("Регистрация увольнения сотрудника с описанием причины."),
            QStringLiteral("proc_dismiss_employee")
        },
        {
            ProcedureId::AssignBrigadeToStage,
            QStringLiteral("Назначить бригаду на этап"),
            QObject::tr("Назначение бригады на этап сборки конкретного экземпляра изделия."),
            QStringLiteral("proc_assign_brigade_to_stage")
        },
        {
            ProcedureId::CompleteStage,
            QStringLiteral("Завершить этап сборки"),
            QObject::tr("Завершение этапа сборки по записи assembly_record."),
            QStringLiteral("proc_complete_stage")
        },
        {
            ProcedureId::RegisterTest,
            QStringLiteral("Регистрация испытания"),
            QObject::tr("Регистрация начала испытания изделия в лаборатории."),
            QStringLiteral("proc_register_test")
        },
        {
            ProcedureId::CompleteTest,
            QStringLiteral("Завершить испытание"),
            QObject::tr("Завершение испытания с указанием результата."),
            QStringLiteral("proc_complete_test")
        },
    };

    int procedureIndex(ProcedureId id)
    {
        return static_cast<int>(id);
    }

    bool roleHasProcedure(UserRole role, ProcedureId id)
    {
        switch (role)
        {
        case UserRole::Admin:
            return true;
        case UserRole::Hr:
            return id == ProcedureId::HireWorker || id == ProcedureId::HireItp
                || id == ProcedureId::TransferEmployee || id == ProcedureId::DismissEmployee;
        default:
            return false;
        }
    }
} // namespace

const ProcedureDefinition& ProcedureCatalog::definition(ProcedureId id)
{
    return kAllProcedures[procedureIndex(id)];
}

QList<ProcedureDefinition> ProcedureCatalog::proceduresForRole(UserRole role)
{
    QList<ProcedureDefinition> result;
    for (const ProcedureDefinition& procedure : kAllProcedures)
    {
        if (roleHasProcedure(role, procedure.id))
        {
            result.append(procedure);
        }
    }
    return result;
}

QString ProcedureCatalog::listItemTitle(const ProcedureDefinition& procedure)
{
    return procedure.shortTitle;
}
