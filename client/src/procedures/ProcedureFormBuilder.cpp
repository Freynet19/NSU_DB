#include "procedures/ProcedureFormBuilder.h"

#include "db/QueryRepository.h"
#include "procedures/ProcedureCatalog.h"
#include "widgets/FilterHelpers.h"
#include "widgets/ProcedureFormWidget.h"

#include <functional>

#include <QDate>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QSpinBox>

namespace {

void addExecuteButton(QFormLayout *form,
                      ProcedureFormWidget *widget,
                      QWidget *messageParent,
                      const QString &text,
                      const QString &successMessage,
                      const std::function<bool(QString *)> &action)
{
    auto *btn = new QPushButton(text, widget);
    form->addRow(btn);
    QObject::connect(btn, &QPushButton::clicked, widget, [messageParent, successMessage, action]() {
        QString err;
        if (!action(&err)) {
            QMessageBox::critical(messageParent, QObject::tr("Ошибка"), err);
        } else {
            QMessageBox::information(messageParent, QObject::tr("Готово"), successMessage);
        }
    });
}

} // namespace

void ProcedureFormBuilder::configure(ProcedureFormWidget *widget,
                                       ProcedureId procedureId,
                                       QueryRepository *repository,
                                       QWidget *messageParent)
{
    widget->clearForm();
    QFormLayout *form = widget->formLayout();

    switch (procedureId) {
    case ProcedureId::HireWorker: {
        auto *name = new QLineEdit(widget);
        auto *birth = addDateFilter(form, QObject::tr("Дата рождения"), QDate(1990, 1, 1), widget);
        auto *hire = addDateFilter(form, QObject::tr("Дата приёма"), QDate::currentDate(), widget);
        auto *cat = new QSpinBox(widget);
        cat->setRange(1, 99);
        auto *spec = new QLineEdit(widget);
        auto *grade = new QSpinBox(widget);
        grade->setRange(1, 8);
        auto *brigade = addOptionalIntFilter(form, QObject::tr("Бригада"), widget);
        form->addRow(QObject::tr("ФИО"), name);
        form->addRow(QObject::tr("Код категории"), cat);
        form->addRow(QObject::tr("Специальность"), spec);
        form->addRow(QObject::tr("Разряд"), grade);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         QObject::tr("Принять рабочего"),
                         QObject::tr("Рабочий принят."),
                         [repository, name, birth, hire, cat, spec, grade, brigade](QString *err) {
                             return repository->callProcHireWorker(
                                 name->text(), birth->date(), hire->date(), cat->value(), spec->text(),
                                 grade->value(), optionalSpinValue(brigade), err);
                         });
        break;
    }
    case ProcedureId::HireItp: {
        auto *name = new QLineEdit(widget);
        auto *birth = addDateFilter(form, QObject::tr("Дата рождения"), QDate(1985, 5, 10), widget);
        auto *hire = addDateFilter(form, QObject::tr("Дата приёма"), QDate::currentDate(), widget);
        auto *cat = new QSpinBox(widget);
        cat->setRange(1, 99);
        auto *pos = new QLineEdit(widget);
        auto *qual = new QLineEdit(widget);
        form->addRow(QObject::tr("ФИО"), name);
        form->addRow(QObject::tr("Код категории"), cat);
        form->addRow(QObject::tr("Должность"), pos);
        form->addRow(QObject::tr("Квалификация"), qual);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         QObject::tr("Принять ИТП"),
                         QObject::tr("ИТП принят."),
                         [repository, name, birth, hire, cat, pos, qual](QString *err) {
                             return repository->callProcHireItp(name->text(), birth->date(), hire->date(),
                                                              cat->value(), pos->text(), qual->text(), err);
                         });
        break;
    }
    case ProcedureId::TransferEmployee: {
        auto *emp = new QSpinBox(widget);
        emp->setRange(1, 99999);
        auto *brigade = addOptionalIntFilter(form, QObject::tr("Новая бригада"), widget);
        auto *section = addOptionalIntFilter(form, QObject::tr("Новый участок (мастер)"), widget);
        auto *desc = new QLineEdit(widget);
        form->addRow(QObject::tr("ID сотрудника"), emp);
        form->addRow(QObject::tr("Описание"), desc);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         QObject::tr("Перевести сотрудника"),
                         QObject::tr("Перевод выполнен."),
                         [repository, emp, brigade, section, desc](QString *err) {
                             return repository->callProcTransferEmployee(
                                 emp->value(), optionalSpinValue(brigade), optionalSpinValue(section),
                                 desc->text(), err);
                         });
        break;
    }
    case ProcedureId::DismissEmployee: {
        auto *emp = new QSpinBox(widget);
        emp->setRange(1, 99999);
        auto *desc = new QLineEdit(widget);
        form->addRow(QObject::tr("ID сотрудника"), emp);
        form->addRow(QObject::tr("Описание"), desc);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         QObject::tr("Уволить сотрудника"),
                         QObject::tr("Увольнение зарегистрировано."),
                         [repository, emp, desc](QString *err) {
                             return repository->callProcDismissEmployee(emp->value(), desc->text(), err);
                         });
        break;
    }
    case ProcedureId::AssignBrigadeToStage: {
        auto *inst = new QSpinBox(widget);
        auto *stage = new QSpinBox(widget);
        auto *brig = new QSpinBox(widget);
        auto *start = addDateFilter(form, QObject::tr("Дата начала"), QDate::currentDate(), widget);
        inst->setRange(1, 99999);
        stage->setRange(1, 99999);
        brig->setRange(1, 99999);
        form->addRow(QObject::tr("ID экземпляра"), inst);
        form->addRow(QObject::tr("ID этапа"), stage);
        form->addRow(QObject::tr("ID бригады"), brig);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         QObject::tr("Назначить бригаду на этап"),
                         QObject::tr("Бригада назначена на этап."),
                         [repository, inst, stage, brig, start](QString *err) {
                             return repository->callProcAssignBrigadeToStage(
                                 inst->value(), stage->value(), brig->value(), start->date(), err);
                         });
        break;
    }
    case ProcedureId::CompleteStage: {
        auto *rec = new QSpinBox(widget);
        auto *end = addDateFilter(form, QObject::tr("Дата завершения"), QDate::currentDate(), widget);
        rec->setRange(1, 99999);
        form->addRow(QObject::tr("ID записи сборки"), rec);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         QObject::tr("Завершить этап сборки"),
                         QObject::tr("Этап завершён."),
                         [repository, rec, end](QString *err) {
                             return repository->callProcCompleteStage(rec->value(), end->date(), err);
                         });
        break;
    }
    case ProcedureId::RegisterTest: {
        auto *inst = new QSpinBox(widget);
        auto *lab = new QSpinBox(widget);
        auto *date = addDateFilter(form, QObject::tr("Дата испытания"), QDate::currentDate(), widget);
        inst->setRange(1, 99999);
        lab->setRange(1, 99999);
        form->addRow(QObject::tr("ID экземпляра"), inst);
        form->addRow(QObject::tr("ID лаборатории"), lab);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         QObject::tr("Зарегистрировать испытание"),
                         QObject::tr("Испытание зарегистрировано."),
                         [repository, inst, lab, date](QString *err) {
                             return repository->callProcRegisterTest(inst->value(), lab->value(),
                                                                     date->date(), err);
                         });
        break;
    }
    case ProcedureId::CompleteTest: {
        auto *testId = new QSpinBox(widget);
        auto *result = new QLineEdit(widget);
        testId->setRange(1, 99999);
        form->addRow(QObject::tr("ID испытания"), testId);
        form->addRow(QObject::tr("Результат"), result);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         QObject::tr("Завершить испытание"),
                         QObject::tr("Испытание завершено."),
                         [repository, testId, result](QString *err) {
                             return repository->callProcCompleteTest(testId->value(), result->text(), err);
                         });
        break;
    }
    }
}
