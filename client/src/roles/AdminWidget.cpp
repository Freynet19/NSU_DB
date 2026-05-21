#include "roles/AdminWidget.h"

#include "db/DatabaseManager.h"
#include "db/QueryRepository.h"
#include "widgets/FilterHelpers.h"
#include "widgets/ReportTableWidget.h"
#include "widgets/TableCrudWidget.h"

#include <QDateEdit>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

AdminWidget::AdminWidget(QWidget *parent)
    : QWidget(parent)
    , m_repo(new QueryRepository(DatabaseManager::instance().connectionName()))
{
    m_tabs = new QTabWidget(this);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_tabs);

    const QStringList tables = {
        QStringLiteral("workshop"),
        QStringLiteral("section"),
        QStringLiteral("brigade"),
        QStringLiteral("personnel_category"),
        QStringLiteral("product_category"),
        QStringLiteral("work_type"),
        QStringLiteral("laboratory"),
        QStringLiteral("equipment"),
        QStringLiteral("employee"),
        QStringLiteral("worker"),
        QStringLiteral("itp"),
        QStringLiteral("product_type"),
        QStringLiteral("product_instance"),
        QStringLiteral("production_cycle"),
        QStringLiteral("assembly_record"),
        QStringLiteral("test"),
        QStringLiteral("test_specialist"),
        QStringLiteral("test_equipment"),
        QStringLiteral("workshop_laboratory"),
        QStringLiteral("personnel_movement"),
    };

    for (const QString &table : tables) {
        m_tabs->addTab(new TableCrudWidget(table, {}, this), table);
    }

    m_tabs->addTab(buildProceduresTab(), tr("Процедуры"));
    m_tabs->addTab(buildReportsTab(), tr("Отчёты (14)"));
}

QWidget *AdminWidget::buildProceduresTab()
{
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    auto *container = new QWidget(scroll);
    auto *layout = new QVBoxLayout(container);

    auto addProcGroup = [&](const QString &title, QWidget *content) {
        auto *box = new QGroupBox(title, container);
        auto *boxLayout = new QVBoxLayout(box);
        boxLayout->addWidget(content);
        layout->addWidget(box);
    };

    // proc_hire_worker
    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *name = new QLineEdit(w);
        auto *birth = addDateFilter(form, tr("Дата рождения"), QDate(1990, 1, 1), w);
        auto *hire = addDateFilter(form, tr("Дата приёма"), QDate::currentDate(), w);
        auto *cat = new QSpinBox(w);
        cat->setRange(1, 99);
        auto *spec = new QLineEdit(w);
        auto *grade = new QSpinBox(w);
        grade->setRange(1, 8);
        auto *brigade = addOptionalIntFilter(form, tr("Бригада"), w);
        form->addRow(tr("ФИО"), name);
        form->addRow(tr("Код категории"), cat);
        form->addRow(tr("Специальность"), spec);
        form->addRow(tr("Разряд"), grade);
        auto *btn = new QPushButton(tr("CALL proc_hire_worker"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [=]() {
            QString err;
            if (!m_repo->callProcHireWorker(name->text(), birth->date(), hire->date(), cat->value(),
                                            spec->text(), grade->value(), optionalSpinValue(brigade),
                                            &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Рабочий принят."));
            }
        });
        addProcGroup(tr("Приём рабочего"), w);
    }

    // proc_hire_itp
    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *name = new QLineEdit(w);
        auto *birth = addDateFilter(form, tr("Дата рождения"), QDate(1985, 5, 10), w);
        auto *hire = addDateFilter(form, tr("Дата приёма"), QDate::currentDate(), w);
        auto *cat = new QSpinBox(w);
        cat->setRange(1, 99);
        auto *pos = new QLineEdit(w);
        auto *qual = new QLineEdit(w);
        form->addRow(tr("ФИО"), name);
        form->addRow(tr("Код категории"), cat);
        form->addRow(tr("Должность"), pos);
        form->addRow(tr("Квалификация"), qual);
        auto *btn = new QPushButton(tr("CALL proc_hire_itp"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [=]() {
            QString err;
            if (!m_repo->callProcHireItp(name->text(), birth->date(), hire->date(), cat->value(),
                                       pos->text(), qual->text(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("ИТП принят."));
            }
        });
        addProcGroup(tr("Приём ИТП"), w);
    }

    // proc_transfer_employee
    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *emp = new QSpinBox(w);
        emp->setRange(1, 99999);
        auto *brigade = addOptionalIntFilter(form, tr("Новая бригада"), w);
        auto *section = addOptionalIntFilter(form, tr("Новый участок (мастер)"), w);
        auto *desc = new QLineEdit(w);
        form->addRow(tr("ID сотрудника"), emp);
        form->addRow(tr("Описание"), desc);
        auto *btn = new QPushButton(tr("CALL proc_transfer_employee"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [=]() {
            QString err;
            if (!m_repo->callProcTransferEmployee(emp->value(), optionalSpinValue(brigade),
                                                  optionalSpinValue(section), desc->text(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Перевод выполнен."));
            }
        });
        addProcGroup(tr("Перевод"), w);
    }

    // proc_dismiss_employee
    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *emp = new QSpinBox(w);
        emp->setRange(1, 99999);
        auto *desc = new QLineEdit(w);
        form->addRow(tr("ID сотрудника"), emp);
        form->addRow(tr("Описание"), desc);
        auto *btn = new QPushButton(tr("CALL proc_dismiss_employee"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [=]() {
            QString err;
            if (!m_repo->callProcDismissEmployee(emp->value(), desc->text(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Увольнение зарегистрировано."));
            }
        });
        addProcGroup(tr("Увольнение"), w);
    }

    // proc_assign_brigade_to_stage
    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *inst = new QSpinBox(w);
        auto *stage = new QSpinBox(w);
        auto *brig = new QSpinBox(w);
        auto *start = addDateFilter(form, tr("Дата начала"), QDate::currentDate(), w);
        inst->setRange(1, 99999);
        stage->setRange(1, 99999);
        brig->setRange(1, 99999);
        form->addRow(tr("ID экземпляра"), inst);
        form->addRow(tr("ID этапа"), stage);
        form->addRow(tr("ID бригады"), brig);
        auto *btn = new QPushButton(tr("CALL proc_assign_brigade_to_stage"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [=]() {
            QString err;
            if (!m_repo->callProcAssignBrigadeToStage(inst->value(), stage->value(), brig->value(),
                                                      start->date(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Бригада назначена на этап."));
            }
        });
        addProcGroup(tr("Назначить бригаду на этап"), w);
    }

    // proc_complete_stage
    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *rec = new QSpinBox(w);
        auto *end = addDateFilter(form, tr("Дата завершения"), QDate::currentDate(), w);
        rec->setRange(1, 99999);
        form->addRow(tr("ID записи сборки"), rec);
        auto *btn = new QPushButton(tr("CALL proc_complete_stage"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [=]() {
            QString err;
            if (!m_repo->callProcCompleteStage(rec->value(), end->date(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Этап завершён."));
            }
        });
        addProcGroup(tr("Завершить этап"), w);
    }

    // proc_register_test
    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *inst = new QSpinBox(w);
        auto *lab = new QSpinBox(w);
        auto *date = addDateFilter(form, tr("Дата испытания"), QDate::currentDate(), w);
        inst->setRange(1, 99999);
        lab->setRange(1, 99999);
        form->addRow(tr("ID экземпляра"), inst);
        form->addRow(tr("ID лаборатории"), lab);
        auto *btn = new QPushButton(tr("CALL proc_register_test"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [=]() {
            QString err;
            if (!m_repo->callProcRegisterTest(inst->value(), lab->value(), date->date(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Испытание зарегистрировано."));
            }
        });
        addProcGroup(tr("Регистрация испытания"), w);
    }

    // proc_complete_test
    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *testId = new QSpinBox(w);
        auto *result = new QLineEdit(w);
        testId->setRange(1, 99999);
        form->addRow(tr("ID испытания"), testId);
        form->addRow(tr("Результат"), result);
        auto *btn = new QPushButton(tr("CALL proc_complete_test"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [=]() {
            QString err;
            if (!m_repo->callProcCompleteTest(testId->value(), result->text(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Испытание завершено."));
            }
        });
        addProcGroup(tr("Завершить испытание"), w);
    }

    layout->addStretch();
    scroll->setWidget(container);
    return scroll;
}

QWidget *AdminWidget::buildReportsTab()
{
    auto *tabs = new QTabWidget(this);

    auto addReport = [&](const QString &title, ReportTableWidget *widget) {
        tabs->addTab(widget, title);
    };

    // Query 1
    {
        auto *r = new ReportTableWidget(tr("1. Виды изделий"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query1ProductTypes(optionalSpinValue(ws), optionalSpinValue(cat), err);
        });
        addReport(tr("1"), r);
    }

    // Query 2
    {
        auto *r = new ReportTableWidget(tr("2. Собранные изделия"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        auto *from = addDateFilter(r->filterLayout(), tr("С"), QDate(2020, 1, 1), r);
        auto *to = addDateFilter(r->filterLayout(), tr("По"), QDate::currentDate(), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query2ProductList(optionalSpinValue(ws), optionalSpinValue(sec),
                                             optionalSpinValue(cat), from->date(), to->date(), err);
        });
        addReport(tr("2"), r);
    }

    // Query 3
    {
        auto *r = new ReportTableWidget(tr("3. Кадровый состав"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        auto *type = addPersonnelTypeFilter(r->filterLayout(), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query3Personnel(optionalSpinValue(ws), optionalSpinValue(sec),
                                           type->currentData().toString(), err);
        });
        addReport(tr("3"), r);
    }

    // Query 4
    {
        auto *r = new ReportTableWidget(tr("4. Участки"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query4SectionList(optionalSpinValue(ws), err);
        });
        addReport(tr("4"), r);
    }

    // Query 5
    {
        auto *r = new ReportTableWidget(tr("5. Работы изделия"), this);
        auto *inst = new QSpinBox(r);
        inst->setRange(1, 99999);
        inst->setValue(1);
        r->filterLayout()->addRow(tr("ID экземпляра"), inst);
        r->setRunReport([=](QString *err) { return m_repo->query5ProductWorks(inst->value(), err); });
        addReport(tr("5"), r);
    }

    // Query 6
    {
        auto *r = new ReportTableWidget(tr("6. Состав бригад"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query6BrigadeComposition(optionalSpinValue(ws), optionalSpinValue(sec), err);
        });
        addReport(tr("6"), r);
    }

    // Query 7
    {
        auto *r = new ReportTableWidget(tr("7. Мастера"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query7SectionMasters(optionalSpinValue(ws), optionalSpinValue(sec), err);
        });
        addReport(tr("7"), r);
    }

    // Query 8
    {
        auto *r = new ReportTableWidget(tr("8. Собираемые сейчас"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query8CurrentProducts(optionalSpinValue(ws), optionalSpinValue(sec),
                                                 optionalSpinValue(cat), err);
        });
        addReport(tr("8"), r);
    }

    // Query 9
    {
        auto *r = new ReportTableWidget(tr("9. Бригады сборки"), this);
        auto *inst = new QSpinBox(r);
        inst->setRange(1, 99999);
        inst->setValue(1);
        r->filterLayout()->addRow(tr("ID экземпляра"), inst);
        r->setRunReport([=](QString *err) { return m_repo->query9ProductBrigades(inst->value(), err); });
        addReport(tr("9"), r);
    }

    // Query 10
    {
        auto *r = new ReportTableWidget(tr("10. Лаборатории изделия"), this);
        auto *inst = new QSpinBox(r);
        inst->setRange(1, 99999);
        inst->setValue(1);
        r->filterLayout()->addRow(tr("ID экземпляра"), inst);
        r->setRunReport([=](QString *err) {
            return m_repo->query10ProductLaboratories(inst->value(), err);
        });
        addReport(tr("10"), r);
    }

    // Query 11
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
        addReport(tr("11"), r);
    }

    // Query 12
    {
        auto *r = new ReportTableWidget(tr("12. Испытатели"), this);
        auto *lab = addOptionalIntFilter(r->filterLayout(), tr("Лаборатория"), r);
        auto *from = addDateFilter(r->filterLayout(), tr("С"), QDate(2020, 1, 1), r);
        auto *to = addDateFilter(r->filterLayout(), tr("По"), QDate::currentDate(), r);
        auto *inst = addOptionalIntFilter(r->filterLayout(), tr("ID экземпляра"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query12TestSpecialists(optionalSpinValue(lab), from->date(), to->date(),
                                                  optionalSpinValue(inst), optionalSpinValue(cat), err);
        });
        addReport(tr("12"), r);
    }

    // Query 13
    {
        auto *r = new ReportTableWidget(tr("13. Оборудование испытаний"), this);
        auto *lab = addOptionalIntFilter(r->filterLayout(), tr("Лаборатория"), r);
        auto *from = addDateFilter(r->filterLayout(), tr("С"), QDate(2020, 1, 1), r);
        auto *to = addDateFilter(r->filterLayout(), tr("По"), QDate::currentDate(), r);
        auto *inst = addOptionalIntFilter(r->filterLayout(), tr("ID экземпляра"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query13TestEquipment(optionalSpinValue(lab), from->date(), to->date(),
                                                optionalSpinValue(inst), optionalSpinValue(cat), err);
        });
        addReport(tr("13"), r);
    }

    // Query 14
    {
        auto *r = new ReportTableWidget(tr("14. Собираемые (список)"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        auto *cat = addOptionalIntFilter(r->filterLayout(), tr("Категория"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query14CurrentList(optionalSpinValue(ws), optionalSpinValue(sec),
                                              optionalSpinValue(cat), err);
        });
        addReport(tr("14"), r);
    }

    return tabs;
}
