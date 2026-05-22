#pragma once

#include "core/UserRole.h"

#include <QList>
#include <QString>

enum class ReportId
{
    ProductTypes = 1,
    FinishedInPeriod = 2,
    Personnel = 3,
    Sections = 4,
    ProductWorks = 5,
    BrigadeComposition = 6,
    SectionMasters = 7,
    CurrentAssembling = 8,
    ProductBrigades = 9,
    ProductLaboratories = 10,
    TestedInLab = 11,
    TestSpecialists = 12,
    TestEquipment = 13,
    CurrentList = 14,
};

struct ReportDefinition
{
    ReportId id;
    QString shortTitle;
    QString description;
};

class ReportCatalog
{
public:
    static const ReportDefinition& definition(ReportId id);
    static QList<ReportDefinition> reportsForRole(UserRole role);
    static QString listItemTitle(const ReportDefinition& report);
    static bool hasCountQuery(ReportId id);
    static QString countSummaryLabel(ReportId id);
};
