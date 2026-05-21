#pragma once

#include "reports/ReportCatalog.h"

class QueryRepository;
class ReportTableWidget;

class ReportFilterBuilder
{
public:
    static void configure(ReportTableWidget *widget,
                          ReportId reportId,
                          QueryRepository *repository);
};
