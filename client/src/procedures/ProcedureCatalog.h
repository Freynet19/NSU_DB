#pragma once

#include "core/UserRole.h"

#include <QList>
#include <QString>

enum class ProcedureId {
    HireWorker,
    HireItp,
    TransferEmployee,
    DismissEmployee,
    AssignBrigadeToStage,
    CompleteStage,
    RegisterTest,
    CompleteTest,
};

struct ProcedureDefinition
{
    ProcedureId id;
    QString shortTitle;
    QString description;
    QString sqlName;
};

class ProcedureCatalog
{
public:
    static const ProcedureDefinition &definition(ProcedureId id);
    static QList<ProcedureDefinition> proceduresForRole(UserRole role);
    static QString listItemTitle(const ProcedureDefinition &procedure);
};
