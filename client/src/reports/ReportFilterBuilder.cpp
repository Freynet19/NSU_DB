#include "reports/ReportFilterBuilder.h"

#include "db/LookupRepository.h"
#include "db/QueryRepository.h"
#include "reports/ReportCatalog.h"
#include "db/LookupRepository.h"
#include "widgets/FilterHelpers.h"
#include "widgets/ReportTableWidget.h"

#include <QComboBox>
#include <QDate>
#include <QSqlQueryModel>

namespace
{
    QComboBox* addInstanceFilter(QFormLayout* form, ReportTableWidget* parent, LookupRepository* lookups)
    {
        return addRequiredEntityCombo(form, QObject::tr("Экземпляр изделия"), parent,
                                      loadLookupItems(lookups, LookupKind::ProductInstance, parent));
    }

    void applyCountAndList(ReportTableWidget* widget,
                           ReportId reportId,
                           QueryRepository* repository,
                           ReportTableWidget::RunReportFn listFn,
                           ReportTableWidget::RunReportFn countFn)
    {
        widget->setRunReport(std::move(listFn));
        if (ReportCatalog::hasCountQuery(reportId))
        {
            widget->setCountCaption(ReportCatalog::countSummaryLabel(reportId));
            widget->setRunCount(std::move(countFn));
        }
        else
        {
            widget->setRunCount({});
            widget->setCountCaption({});
        }
    }
} // namespace

void ReportFilterBuilder::configure(ReportTableWidget* widget,
                                    ReportId reportId,
                                    QueryRepository* repository,
                                    LookupRepository* lookups)
{
    widget->clearFilters();

    QFormLayout* form = widget->filterLayout();

    switch (reportId)
    {
    case ReportId::ProductTypes:
        {
            auto* ws = addOptionalEntityCombo(form, QObject::tr("Цех"), widget,
                                              loadLookupItems(lookups, LookupKind::Workshop, widget));
            auto* cat = addOptionalEntityCombo(form, QObject::tr("Категория"), widget,
                                               loadLookupItems(lookups, LookupKind::ProductCategory, widget));
            widget->setRunReport([repository, ws, cat](QString* err)
            {
                return repository->query1ProductTypes(optionalComboValue(ws), optionalComboValue(cat), err);
            });
            break;
        }
    case ReportId::FinishedInPeriod:
        {
            auto* ws = addOptionalEntityCombo(form, QObject::tr("Цех"), widget,
                                              loadLookupItems(lookups, LookupKind::Workshop, widget));
            auto* sec = addOptionalEntityCombo(form, QObject::tr("Участок"), widget,
                                               loadLookupItems(lookups, LookupKind::Section, widget));
            auto* cat = addOptionalEntityCombo(form, QObject::tr("Категория"), widget,
                                               loadLookupItems(lookups, LookupKind::ProductCategory, widget));
            auto* from = addDateFilter(form, QObject::tr("С"), QDate(2020, 1, 1), widget);
            auto* to = addDateFilter(form, QObject::tr("По"), QDate::currentDate(), widget);
            applyCountAndList(
                widget,
                reportId,
                repository,
                [repository, ws, sec, cat, from, to](QString* err)
                {
                    return repository->query2ProductList(optionalComboValue(ws), optionalComboValue(sec),
                                                         optionalComboValue(cat), from->date(),
                                                         to->date(), err);
                },
                [repository, ws, sec, cat, from, to](QString* err)
                {
                    return repository->query2ProductCount(optionalComboValue(ws), optionalComboValue(sec),
                                                          optionalComboValue(cat), from->date(),
                                                          to->date(), err);
                });
            break;
        }
    case ReportId::Personnel:
        {
            auto* ws = addOptionalEntityCombo(form, QObject::tr("Цех"), widget,
                                              loadLookupItems(lookups, LookupKind::Workshop, widget));
            auto* cat = addOptionalEntityCombo(form, QObject::tr("Категория персонала"), widget,
                                               loadLookupItems(lookups, LookupKind::PersonnelCategory, widget));
            auto* type = addPersonnelTypeFilter(form, widget);
            widget->setRunReport([repository, ws, cat, type](QString* err)
            {
                return repository->query3Personnel(optionalComboValue(ws), optionalComboValue(cat),
                                                   type->currentData().toString(), err);
            });
            break;
        }
    case ReportId::Sections:
        {
            auto* ws = addOptionalEntityCombo(form, QObject::tr("Цех"), widget,
                                              loadLookupItems(lookups, LookupKind::Workshop, widget));
            applyCountAndList(
                widget,
                reportId,
                repository,
                [repository, ws](QString* err)
                {
                    return repository->query4SectionList(optionalComboValue(ws), err);
                },
                [repository, ws](QString* err)
                {
                    return repository->query4SectionCount(optionalComboValue(ws), err);
                });
            break;
        }
    case ReportId::ProductWorks:
        {
            auto* inst = addInstanceFilter(form, widget, lookups);
            widget->setRunReport([repository, inst](QString* err) -> QSqlQueryModel*
            {
                const std::optional<int> instanceId =
                    requireComboValue(inst, QObject::tr("Экземпляр изделия"), err);
                if (!instanceId)
                {
                    return nullptr;
                }
                return repository->query5ProductWorks(*instanceId, err);
            });
            break;
        }
    case ReportId::BrigadeComposition:
        {
            auto* ws = addOptionalEntityCombo(form, QObject::tr("Цех"), widget,
                                              loadLookupItems(lookups, LookupKind::Workshop, widget));
            auto* sec = addOptionalEntityCombo(form, QObject::tr("Участок"), widget,
                                               loadLookupItems(lookups, LookupKind::Section, widget));
            widget->setRunReport([repository, ws, sec](QString* err)
            {
                return repository->query6BrigadeComposition(optionalComboValue(ws), optionalComboValue(sec),
                                                            err);
            });
            break;
        }
    case ReportId::SectionMasters:
        {
            auto* ws = addOptionalEntityCombo(form, QObject::tr("Цех"), widget,
                                              loadLookupItems(lookups, LookupKind::Workshop, widget));
            auto* sec = addOptionalEntityCombo(form, QObject::tr("Участок"), widget,
                                               loadLookupItems(lookups, LookupKind::Section, widget));
            widget->setRunReport([repository, ws, sec](QString* err)
            {
                return repository->query7SectionMasters(optionalComboValue(ws), optionalComboValue(sec), err);
            });
            break;
        }
    case ReportId::CurrentAssembling:
        {
            auto* ws = addOptionalEntityCombo(form, QObject::tr("Цех"), widget,
                                              loadLookupItems(lookups, LookupKind::Workshop, widget));
            auto* sec = addOptionalEntityCombo(form, QObject::tr("Участок"), widget,
                                               loadLookupItems(lookups, LookupKind::Section, widget));
            auto* cat = addOptionalEntityCombo(form, QObject::tr("Категория"), widget,
                                               loadLookupItems(lookups, LookupKind::ProductCategory, widget));
            widget->setRunReport([repository, ws, sec, cat](QString* err)
            {
                return repository->query8CurrentProducts(optionalComboValue(ws), optionalComboValue(sec),
                                                         optionalComboValue(cat), err);
            });
            break;
        }
    case ReportId::ProductBrigades:
        {
            auto* inst = addInstanceFilter(form, widget, lookups);
            widget->setRunReport([repository, inst](QString* err) -> QSqlQueryModel*
            {
                const std::optional<int> instanceId =
                    requireComboValue(inst, QObject::tr("Экземпляр изделия"), err);
                if (!instanceId)
                {
                    return nullptr;
                }
                return repository->query9ProductBrigades(*instanceId, err);
            });
            break;
        }
    case ReportId::ProductLaboratories:
        {
            auto* inst = addInstanceFilter(form, widget, lookups);
            widget->setRunReport([repository, inst](QString* err) -> QSqlQueryModel*
            {
                const std::optional<int> instanceId =
                    requireComboValue(inst, QObject::tr("Экземпляр изделия"), err);
                if (!instanceId)
                {
                    return nullptr;
                }
                return repository->query10ProductLaboratories(*instanceId, err);
            });
            break;
        }
    case ReportId::TestedInLab:
        {
            auto* lab = addRequiredEntityCombo(form, QObject::tr("Лаборатория"), widget,
                                               loadLookupItems(lookups, LookupKind::Laboratory, widget));
            auto* cat = addOptionalEntityCombo(form, QObject::tr("Категория"), widget,
                                               loadLookupItems(lookups, LookupKind::ProductCategory, widget));
            auto* from = addDateFilter(form, QObject::tr("С"), QDate(2020, 1, 1), widget);
            auto* to = addDateFilter(form, QObject::tr("По"), QDate::currentDate(), widget);
            widget->setRunReport([repository, lab, cat, from, to](QString* err) -> QSqlQueryModel*
            {
                const std::optional<int> laboratoryId =
                    requireComboValue(lab, QObject::tr("Лаборатория"), err);
                if (!laboratoryId)
                {
                    return nullptr;
                }
                return repository->query11TestedProducts(laboratoryId, optionalComboValue(cat),
                                                         from->date(), to->date(), err);
            });
            break;
        }
    case ReportId::TestSpecialists:
        {
            auto* lab = addOptionalEntityCombo(form, QObject::tr("Лаборатория"), widget,
                                               loadLookupItems(lookups, LookupKind::Laboratory, widget));
            auto* from = addDateFilter(form, QObject::tr("С"), QDate(2020, 1, 1), widget);
            auto* to = addDateFilter(form, QObject::tr("По"), QDate::currentDate(), widget);
            auto* inst = addOptionalEntityCombo(form, QObject::tr("Экземпляр изделия"), widget,
                                                loadLookupItems(lookups, LookupKind::ProductInstance, widget));
            auto* cat = addOptionalEntityCombo(form, QObject::tr("Категория"), widget,
                                               loadLookupItems(lookups, LookupKind::ProductCategory, widget));
            widget->setRunReport([repository, lab, from, to, inst, cat](QString* err)
            {
                return repository->query12TestSpecialists(optionalComboValue(lab), from->date(),
                                                          to->date(), optionalComboValue(inst),
                                                          optionalComboValue(cat), err);
            });
            break;
        }
    case ReportId::TestEquipment:
        {
            auto* lab = addOptionalEntityCombo(form, QObject::tr("Лаборатория"), widget,
                                               loadLookupItems(lookups, LookupKind::Laboratory, widget));
            auto* from = addDateFilter(form, QObject::tr("С"), QDate(2020, 1, 1), widget);
            auto* to = addDateFilter(form, QObject::tr("По"), QDate::currentDate(), widget);
            auto* inst = addOptionalEntityCombo(form, QObject::tr("Экземпляр изделия"), widget,
                                                loadLookupItems(lookups, LookupKind::ProductInstance, widget));
            auto* cat = addOptionalEntityCombo(form, QObject::tr("Категория"), widget,
                                               loadLookupItems(lookups, LookupKind::ProductCategory, widget));
            widget->setRunReport([repository, lab, from, to, inst, cat](QString* err)
            {
                return repository->query13TestEquipment(optionalComboValue(lab), from->date(),
                                                        to->date(), optionalComboValue(inst),
                                                        optionalComboValue(cat), err);
            });
            break;
        }
    case ReportId::CurrentList:
        {
            auto* ws = addOptionalEntityCombo(form, QObject::tr("Цех"), widget,
                                              loadLookupItems(lookups, LookupKind::Workshop, widget));
            auto* sec = addOptionalEntityCombo(form, QObject::tr("Участок"), widget,
                                               loadLookupItems(lookups, LookupKind::Section, widget));
            auto* cat = addOptionalEntityCombo(form, QObject::tr("Категория"), widget,
                                               loadLookupItems(lookups, LookupKind::ProductCategory, widget));
            applyCountAndList(
                widget,
                reportId,
                repository,
                [repository, ws, sec, cat](QString* err)
                {
                    return repository->query14CurrentList(optionalComboValue(ws), optionalComboValue(sec),
                                                          optionalComboValue(cat), err);
                },
                [repository, ws, sec, cat](QString* err)
                {
                    return repository->query14CurrentCount(optionalComboValue(ws), optionalComboValue(sec),
                                                           optionalComboValue(cat), err);
                });
            break;
        }
    }
}
