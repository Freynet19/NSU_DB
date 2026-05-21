#include "roles/ProductionReportWidget.h"

#include "db/DatabaseManager.h"
#include "db/QueryRepository.h"
#include "widgets/FilterHelpers.h"
#include "widgets/ReportTableWidget.h"

#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

ProductionReportWidget::ProductionReportWidget(QWidget *parent)
    : QWidget(parent)
    , m_repo(new QueryRepository(DatabaseManager::instance().database()))
{
    auto *tabs = new QTabWidget(this);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(tabs);

    // 1 - product types
    {
        auto *r = new ReportTableWidget(tr("1. Виды изделий"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query1ProductTypes(optionalSpinValue(ws), optionalSpinValue(cat), err);
        });
        tabs->addTab(r, tr("1"));
    }

    // 2 - finished products
    {
        auto *r = new ReportTableWidget(tr("2. Собранные за период"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        auto *from = addDateFilter(r->filterLayout(), tr("С"), QDate(2020, 1, 1), r);
        auto *to = addDateFilter(r->filterLayout(), tr("По"), QDate::currentDate(), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query2ProductList(optionalSpinValue(ws), optionalSpinValue(sec),
                                             optionalSpinValue(cat), from->date(), to->date(), err);
        });
        tabs->addTab(r, tr("2"));
    }

    // 8 - current assembling
    {
        auto *r = new ReportTableWidget(tr("8. Собираемые сейчас"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query8CurrentProducts(optionalSpinValue(ws), optionalSpinValue(sec),
                                                 optionalSpinValue(cat), err);
        });
        tabs->addTab(r, tr("8"));
    }

    // 10 - labs for product
    {
        auto *r = new ReportTableWidget(tr("10. Лаборатории изделия"), this);
        auto *inst = new QSpinBox(r);
        inst->setRange(1, 99999);
        inst->setValue(1);
        r->filterLayout()->addRow(tr("ID экземпляра"), inst);
        r->setRunReport([=](QString *err) {
            return m_repo->query10ProductLaboratories(inst->value(), err);
        });
        tabs->addTab(r, tr("10"));
    }

    // 11 - tested in lab
    {
        auto *r = new ReportTableWidget(tr("11. Испытанные в лаборатории"), this);
        auto *lab = addOptionalIntFilter(r->filterLayout(), tr("Лаборатория"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        auto *from = addDateFilter(r->filterLayout(), tr("С"), QDate(2020, 1, 1), r);
        auto *to = addDateFilter(r->filterLayout(), tr("По"), QDate::currentDate(), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query11TestedProducts(optionalSpinValue(lab), optionalSpinValue(cat),
                                                 from->date(), to->date(), err);
        });
        tabs->addTab(r, tr("11"));
    }

    // 14 - current count + list
    {
        auto *r = new ReportTableWidget(tr("14. Собираемые (список)"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query14CurrentList(optionalSpinValue(ws), optionalSpinValue(sec),
                                              optionalSpinValue(cat), err);
        });
        tabs->addTab(r, tr("14"));
    }
}
