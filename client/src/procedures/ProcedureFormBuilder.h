#pragma once

#include "procedures/ProcedureCatalog.h"

class ProcedureFormWidget;
class QueryRepository;
class QWidget;

class ProcedureFormBuilder
{
public:
    static void configure(ProcedureFormWidget *widget,
                          ProcedureId procedureId,
                          QueryRepository *repository,
                          QWidget *messageParent);
};
