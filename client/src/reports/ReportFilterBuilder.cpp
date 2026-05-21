#include "reports/ReportFilterBuilder.h"

#include "db/QueryRepository.h"
#include "widgets/FilterHelpers.h"
#include "widgets/ReportTableWidget.h"

#include <QDate>
#include <QSpinBox>

namespace {

QSpinBox *addInstanceFilter(QFormLayout *form, ReportTableWidget *parent)
{
    auto *inst = new QSpinBox(parent);
    inst->setRange(1, 99999);
    inst->setValue(1);
    form->addRow(QObject::tr("ID экземпляра"), inst);
    return inst;
}

} // namespace

void ReportFilterBuilder::configure(ReportTableWidget *widget,
                                    ReportId reportId,
                                    QueryRepository *repository)
{
    widget->clearFilters();

    QFormLayout *form = widget->filterLayout();

    switch (reportId) {
    case ReportId::ProductTypes: {
        auto *ws = addOptionalIntFilter(form, QObject::tr("Цех"), widget);
        auto *cat = addOptionalIntFilter(form, QObject::tr("Категория"), widget);
        widget->setRunReport([repository, ws, cat](QString *err) {
            return repository->query1ProductTypes(optionalSpinValue(ws), optionalSpinValue(cat), err);
        });
        break;
    }
    case ReportId::FinishedInPeriod: {
        auto *ws = addOptionalIntFilter(form, QObject::tr("Цех"), widget);
        auto *sec = addOptionalIntFilter(form, QObject::tr("Участок"), widget);
        auto *cat = addOptionalIntFilter(form, QObject::tr("Категория"), widget);
        auto *from = addDateFilter(form, QObject::tr("С"), QDate(2020, 1, 1), widget);
        auto *to = addDateFilter(form, QObject::tr("По"), QDate::currentDate(), widget);
        widget->setRunReport([repository, ws, sec, cat, from, to](QString *err) {
            return repository->query2ProductList(optionalSpinValue(ws), optionalSpinValue(sec),
                                                 optionalSpinValue(cat), from->date(), to->date(),
                                                 err);
        });
        break;
    }
    case ReportId::Personnel: {
        auto *ws = addOptionalIntFilter(form, QObject::tr("Цех"), widget);
        auto *sec = addOptionalIntFilter(form, QObject::tr("Участок"), widget);
        auto *type = addPersonnelTypeFilter(form, widget);
        widget->setRunReport([repository, ws, sec, type](QString *err) {
            return repository->query3Personnel(optionalSpinValue(ws), optionalSpinValue(sec),
                                               type->currentData().toString(), err);
        });
        break;
    }
    case ReportId::Sections: {
        auto *ws = addOptionalIntFilter(form, QObject::tr("Цех"), widget);
        widget->setRunReport([repository, ws](QString *err) {
            return repository->query4SectionList(optionalSpinValue(ws), err);
        });
        break;
    }
    case ReportId::ProductWorks: {
        auto *inst = addInstanceFilter(form, widget);
        widget->setRunReport([repository, inst](QString *err) {
            return repository->query5ProductWorks(inst->value(), err);
        });
        break;
    }
    case ReportId::BrigadeComposition: {
        auto *ws = addOptionalIntFilter(form, QObject::tr("Цех"), widget);
        auto *sec = addOptionalIntFilter(form, QObject::tr("Участок"), widget);
        widget->setRunReport([repository, ws, sec](QString *err) {
            return repository->query6BrigadeComposition(optionalSpinValue(ws), optionalSpinValue(sec),
                                                        err);
        });
        break;
    }
    case ReportId::SectionMasters: {
        auto *ws = addOptionalIntFilter(form, QObject::tr("Цех"), widget);
        auto *sec = addOptionalIntFilter(form, QObject::tr("Участок"), widget);
        widget->setRunReport([repository, ws, sec](QString *err) {
            return repository->query7SectionMasters(optionalSpinValue(ws), optionalSpinValue(sec), err);
        });
        break;
    }
    case ReportId::CurrentAssembling: {
        auto *ws = addOptionalIntFilter(form, QObject::tr("Цех"), widget);
        auto *sec = addOptionalIntFilter(form, QObject::tr("Участок"), widget);
        auto *cat = addOptionalIntFilter(form, QObject::tr("Категория"), widget);
        widget->setRunReport([repository, ws, sec, cat](QString *err) {
            return repository->query8CurrentProducts(optionalSpinValue(ws), optionalSpinValue(sec),
                                                     optionalSpinValue(cat), err);
        });
        break;
    }
    case ReportId::ProductBrigades: {
        auto *inst = addInstanceFilter(form, widget);
        widget->setRunReport([repository, inst](QString *err) {
            return repository->query9ProductBrigades(inst->value(), err);
        });
        break;
    }
    case ReportId::ProductLaboratories: {
        auto *inst = addInstanceFilter(form, widget);
        widget->setRunReport([repository, inst](QString *err) {
            return repository->query10ProductLaboratories(inst->value(), err);
        });
        break;
    }
    case ReportId::TestedInLab: {
        auto *lab = addOptionalIntFilter(form, QObject::tr("Лаборатория"), widget);
        auto *cat = addOptionalIntFilter(form, QObject::tr("Категория"), widget);
        auto *from = addDateFilter(form, QObject::tr("С"), QDate(2020, 1, 1), widget);
        auto *to = addDateFilter(form, QObject::tr("По"), QDate::currentDate(), widget);
        widget->setRunReport([repository, lab, cat, from, to](QString *err) {
            return repository->query11TestedProducts(optionalSpinValue(lab), optionalSpinValue(cat),
                                                     from->date(), to->date(), err);
        });
        break;
    }
    case ReportId::TestSpecialists: {
        auto *lab = addOptionalIntFilter(form, QObject::tr("Лаборатория"), widget);
        auto *from = addDateFilter(form, QObject::tr("С"), QDate(2020, 1, 1), widget);
        auto *to = addDateFilter(form, QObject::tr("По"), QDate::currentDate(), widget);
        auto *inst = addOptionalIntFilter(form, QObject::tr("ID экземпляра"), widget);
        auto *cat = addOptionalIntFilter(form, QObject::tr("Категория"), widget);
        widget->setRunReport([repository, lab, from, to, inst, cat](QString *err) {
            return repository->query12TestSpecialists(optionalSpinValue(lab), from->date(),
                                                      to->date(), optionalSpinValue(inst),
                                                      optionalSpinValue(cat), err);
        });
        break;
    }
    case ReportId::TestEquipment: {
        auto *lab = addOptionalIntFilter(form, QObject::tr("Лаборатория"), widget);
        auto *from = addDateFilter(form, QObject::tr("С"), QDate(2020, 1, 1), widget);
        auto *to = addDateFilter(form, QObject::tr("По"), QDate::currentDate(), widget);
        auto *inst = addOptionalIntFilter(form, QObject::tr("ID экземпляра"), widget);
        auto *cat = addOptionalIntFilter(form, QObject::tr("Категория"), widget);
        widget->setRunReport([repository, lab, from, to, inst, cat](QString *err) {
            return repository->query13TestEquipment(optionalSpinValue(lab), from->date(),
                                                      to->date(), optionalSpinValue(inst),
                                                      optionalSpinValue(cat), err);
        });
        break;
    }
    case ReportId::CurrentList: {
        auto *ws = addOptionalIntFilter(form, QObject::tr("Цех"), widget);
        auto *sec = addOptionalIntFilter(form, QObject::tr("Участок"), widget);
        auto *cat = addOptionalIntFilter(form, QObject::tr("Категория"), widget);
        widget->setRunReport([repository, ws, sec, cat](QString *err) {
            return repository->query14CurrentList(optionalSpinValue(ws), optionalSpinValue(sec),
                                                  optionalSpinValue(cat), err);
        });
        break;
    }
    }
}
