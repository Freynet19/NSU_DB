#include "procedures/ProcedureFormBuilder.h"

#include "db/LookupRepository.h"
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
                      LookupRepository *lookups,
                      const QString &text,
                      const QString &successMessage,
                      const std::function<bool(QString *)> &action)
{
    auto *btn = new QPushButton(text, widget);
    form->addRow(btn);
    QObject::connect(btn, &QPushButton::clicked, widget, [messageParent, lookups, successMessage, action]() {
        QString err;
        if (!action(&err)) {
            QMessageBox::critical(messageParent, QObject::tr("Ошибка"), err);
        } else {
            if (lookups) {
                lookups->clearCache();
            }
            QMessageBox::information(messageParent, QObject::tr("Готово"), successMessage);
        }
    });
}

} // namespace

void ProcedureFormBuilder::configure(ProcedureFormWidget *widget,
                                     ProcedureId procedureId,
                                     QueryRepository *repository,
                                     LookupRepository *lookups,
                                     QWidget *messageParent)
{
    widget->clearForm();
    QFormLayout *form = widget->formLayout();

    switch (procedureId) {
    case ProcedureId::HireWorker: {
        auto *name = new QLineEdit(widget);
        auto *birth = addDateFilter(form, QObject::tr("Дата рождения"), QDate(1990, 1, 1), widget);
        auto *hire = addDateFilter(form, QObject::tr("Дата приёма"), QDate::currentDate(), widget);
        auto *cat = addRequiredEntityCombo(form, QObject::tr("Категория персонала"), widget,
                                           lookups->items(LookupKind::PersonnelCategory));
        auto *spec = new QLineEdit(widget);
        auto *grade = new QSpinBox(widget);
        grade->setRange(1, 8);
        auto *brigade = addOptionalEntityCombo(form, QObject::tr("Бригада"), widget,
                                               lookups->items(LookupKind::Brigade));
        form->addRow(QObject::tr("ФИО"), name);
        form->addRow(QObject::tr("Специальность"), spec);
        form->addRow(QObject::tr("Разряд"), grade);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         lookups,
                         QObject::tr("Принять рабочего"),
                         QObject::tr("Рабочий принят."),
                         [repository, name, birth, hire, cat, spec, grade, brigade](QString *err) {
                             const std::optional<int> categoryCode =
                                 requireComboValue(cat, QObject::tr("Категория персонала"), err);
                             if (!categoryCode) {
                                 return false;
                             }
                             return repository->callProcHireWorker(
                                 name->text(), birth->date(), hire->date(), *categoryCode,
                                 spec->text(), grade->value(), optionalComboValue(brigade), err);
                         });
        break;
    }
    case ProcedureId::HireItp: {
        auto *name = new QLineEdit(widget);
        auto *birth = addDateFilter(form, QObject::tr("Дата рождения"), QDate(1985, 5, 10), widget);
        auto *hire = addDateFilter(form, QObject::tr("Дата приёма"), QDate::currentDate(), widget);
        auto *cat = addRequiredEntityCombo(form, QObject::tr("Категория персонала"), widget,
                                           lookups->items(LookupKind::PersonnelCategory));
        auto *pos = new QLineEdit(widget);
        auto *qual = new QLineEdit(widget);
        form->addRow(QObject::tr("ФИО"), name);
        form->addRow(QObject::tr("Должность"), pos);
        form->addRow(QObject::tr("Квалификация"), qual);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         lookups,
                         QObject::tr("Принять ИТП"),
                         QObject::tr("ИТП принят."),
                         [repository, name, birth, hire, cat, pos, qual](QString *err) {
                             const std::optional<int> categoryCode =
                                 requireComboValue(cat, QObject::tr("Категория персонала"), err);
                             if (!categoryCode) {
                                 return false;
                             }
                             return repository->callProcHireItp(name->text(), birth->date(), hire->date(),
                                                              *categoryCode, pos->text(), qual->text(),
                                                              err);
                         });
        break;
    }
    case ProcedureId::TransferEmployee: {
        auto *emp = addRequiredEntityCombo(form, QObject::tr("Сотрудник"), widget,
                                         lookups->items(LookupKind::Employee));
        auto *brigade = addOptionalEntityCombo(form, QObject::tr("Новая бригада"), widget,
                                               lookups->items(LookupKind::Brigade));
        auto *section = addOptionalEntityCombo(form, QObject::tr("Новый участок (мастер)"), widget,
                                               lookups->items(LookupKind::Section));
        auto *desc = new QLineEdit(widget);
        form->addRow(QObject::tr("Описание"), desc);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         lookups,
                         QObject::tr("Перевести сотрудника"),
                         QObject::tr("Перевод выполнен."),
                         [repository, emp, brigade, section, desc](QString *err) {
                             const std::optional<int> employeeId =
                                 requireComboValue(emp, QObject::tr("Сотрудник"), err);
                             if (!employeeId) {
                                 return false;
                             }
                             return repository->callProcTransferEmployee(
                                 *employeeId, optionalComboValue(brigade),
                                 optionalComboValue(section), desc->text(), err);
                         });
        break;
    }
    case ProcedureId::DismissEmployee: {
        auto *emp = addRequiredEntityCombo(form, QObject::tr("Сотрудник"), widget,
                                         lookups->items(LookupKind::Employee));
        auto *desc = new QLineEdit(widget);
        form->addRow(QObject::tr("Описание"), desc);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         lookups,
                         QObject::tr("Уволить сотрудника"),
                         QObject::tr("Увольнение зарегистрировано."),
                         [repository, emp, desc](QString *err) {
                             const std::optional<int> employeeId =
                                 requireComboValue(emp, QObject::tr("Сотрудник"), err);
                             if (!employeeId) {
                                 return false;
                             }
                             return repository->callProcDismissEmployee(*employeeId, desc->text(),
                                                                      err);
                         });
        break;
    }
    case ProcedureId::AssignBrigadeToStage: {
        auto *inst = addRequiredEntityCombo(form, QObject::tr("Экземпляр изделия"), widget,
                                          lookups->items(LookupKind::ProductInstance));
        auto *stage = addRequiredEntityCombo(form, QObject::tr("Этап производства"), widget,
                                           lookups->items(LookupKind::ProductionStage));
        auto *brig = addRequiredEntityCombo(form, QObject::tr("Бригада"), widget,
                                            lookups->items(LookupKind::Brigade));
        auto *start = addDateFilter(form, QObject::tr("Дата начала"), QDate::currentDate(), widget);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         lookups,
                         QObject::tr("Назначить бригаду на этап"),
                         QObject::tr("Бригада назначена на этап."),
                         [repository, inst, stage, brig, start](QString *err) {
                             const std::optional<int> instanceId =
                                 requireComboValue(inst, QObject::tr("Экземпляр изделия"), err);
                             const std::optional<int> stageId =
                                 requireComboValue(stage, QObject::tr("Этап производства"), err);
                             const std::optional<int> brigadeId =
                                 requireComboValue(brig, QObject::tr("Бригада"), err);
                             if (!instanceId || !stageId || !brigadeId) {
                                 return false;
                             }
                             return repository->callProcAssignBrigadeToStage(
                                 *instanceId, *stageId, *brigadeId, start->date(), err);
                         });
        break;
    }
    case ProcedureId::CompleteStage: {
        auto *rec = addRequiredEntityCombo(form, QObject::tr("Запись сборки"), widget,
                                         lookups->items(LookupKind::AssemblyRecord));
        auto *end = addDateFilter(form, QObject::tr("Дата завершения"), QDate::currentDate(), widget);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         lookups,
                         QObject::tr("Завершить этап сборки"),
                         QObject::tr("Этап завершён."),
                         [repository, rec, end](QString *err) {
                             const std::optional<int> recordId =
                                 requireComboValue(rec, QObject::tr("Запись сборки"), err);
                             if (!recordId) {
                                 return false;
                             }
                             return repository->callProcCompleteStage(*recordId, end->date(), err);
                         });
        break;
    }
    case ProcedureId::RegisterTest: {
        auto *inst = addRequiredEntityCombo(form, QObject::tr("Экземпляр изделия"), widget,
                                          lookups->items(LookupKind::ProductInstance));
        auto *lab = addRequiredEntityCombo(form, QObject::tr("Лаборатория"), widget,
                                           lookups->items(LookupKind::Laboratory));
        auto *date = addDateFilter(form, QObject::tr("Дата испытания"), QDate::currentDate(), widget);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         lookups,
                         QObject::tr("Зарегистрировать испытание"),
                         QObject::tr("Испытание зарегистрировано."),
                         [repository, inst, lab, date](QString *err) {
                             const std::optional<int> instanceId =
                                 requireComboValue(inst, QObject::tr("Экземпляр изделия"), err);
                             const std::optional<int> laboratoryId =
                                 requireComboValue(lab, QObject::tr("Лаборатория"), err);
                             if (!instanceId || !laboratoryId) {
                                 return false;
                             }
                             return repository->callProcRegisterTest(*instanceId, *laboratoryId,
                                                                     date->date(), err);
                         });
        break;
    }
    case ProcedureId::CompleteTest: {
        auto *testId = addRequiredEntityCombo(form, QObject::tr("Испытание"), widget,
                                              lookups->items(LookupKind::Test));
        auto *result = new QLineEdit(widget);
        form->addRow(QObject::tr("Результат"), result);
        addExecuteButton(form,
                         widget,
                         messageParent,
                         lookups,
                         QObject::tr("Завершить испытание"),
                         QObject::tr("Испытание завершено."),
                         [repository, testId, result](QString *err) {
                             const std::optional<int> id =
                                 requireComboValue(testId, QObject::tr("Испытание"), err);
                             if (!id) {
                                 return false;
                             }
                             return repository->callProcCompleteTest(*id, result->text(), err);
                         });
        break;
    }
    }
}
