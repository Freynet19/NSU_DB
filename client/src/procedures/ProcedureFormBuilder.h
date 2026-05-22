#pragma once

#include "procedures/ProcedureCatalog.h"

class LookupRepository;
class ProcedureFormWidget;
class QueryRepository;
class QWidget;

class ProcedureFormBuilder
{
public:
    static void configure(ProcedureFormWidget* widget,
                          ProcedureId procedureId,
                          QueryRepository* repository,
                          LookupRepository* lookups,
                          QWidget* messageParent);
};
