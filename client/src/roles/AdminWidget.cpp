#include "roles/AdminWidget.h"

#include "core/UserRole.h"
#include "db/DatabaseManager.h"
#include "db/QueryRepository.h"
#include "widgets/FilterHelpers.h"
#include "widgets/ReportsHubWidget.h"
#include "widgets/TableCrudWidget.h"

#include <QDateEdit>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

namespace {

QString tableDisplayName(const QString &table)
{
    static const QMap<QString, QString> names = {
        {QStringLiteral("workshop"), QStringLiteral("Цех")},
        {QStringLiteral("section"), QStringLiteral("Участок")},
        {QStringLiteral("brigade"), QStringLiteral("Бригада")},
        {QStringLiteral("personnel_category"), QStringLiteral("Категория персонала")},
        {QStringLiteral("product_category"), QStringLiteral("Категория изделий")},
        {QStringLiteral("work_type"), QStringLiteral("Вид работ")},
        {QStringLiteral("laboratory"), QStringLiteral("Лаборатория")},
        {QStringLiteral("equipment"), QStringLiteral("Оборудование")},
        {QStringLiteral("employee"), QStringLiteral("Сотрудник")},
        {QStringLiteral("worker"), QStringLiteral("Рабочий")},
        {QStringLiteral("itp"), QStringLiteral("ИТП")},
        {QStringLiteral("product_type"), QStringLiteral("Вид изделия")},
        {QStringLiteral("product_instance"), QStringLiteral("Экземпляр изделия")},
        {QStringLiteral("production_cycle"), QStringLiteral("Производственный цикл")},
        {QStringLiteral("assembly_record"), QStringLiteral("Запись сборки")},
        {QStringLiteral("test"), QStringLiteral("Испытание")},
        {QStringLiteral("test_specialist"), QStringLiteral("Испытатель")},
        {QStringLiteral("test_equipment"), QStringLiteral("Оборудование испытания")},
        {QStringLiteral("workshop_laboratory"), QStringLiteral("Лаборатория цеха")},
        {QStringLiteral("personnel_movement"), QStringLiteral("Кадровое движение")},
    };
    return names.value(table, table);
}

} // namespace

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
        m_tabs->addTab(new TableCrudWidget(table, {}, this), tableDisplayName(table));
    }

    m_tabs->addTab(buildProceduresTab(), tr("Процедуры"));
    m_tabs->addTab(buildReportsTab(), tr("Отчёты"));
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
        auto *btn = new QPushButton(tr("Принять рабочего"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [this, name, birth, hire, cat, spec, grade, brigade]() {
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
        auto *btn = new QPushButton(tr("Принять ИТП"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [this, name, birth, hire, cat, pos, qual]() {
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
        auto *btn = new QPushButton(tr("Перевести сотрудника"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [this, emp, brigade, section, desc]() {
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

    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *emp = new QSpinBox(w);
        emp->setRange(1, 99999);
        auto *desc = new QLineEdit(w);
        form->addRow(tr("ID сотрудника"), emp);
        form->addRow(tr("Описание"), desc);
        auto *btn = new QPushButton(tr("Уволить сотрудника"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [this, emp, desc]() {
            QString err;
            if (!m_repo->callProcDismissEmployee(emp->value(), desc->text(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Увольнение зарегистрировано."));
            }
        });
        addProcGroup(tr("Увольнение"), w);
    }

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
        auto *btn = new QPushButton(tr("Назначить бригаду на этап"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [this, inst, stage, brig, start]() {
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

    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *rec = new QSpinBox(w);
        auto *end = addDateFilter(form, tr("Дата завершения"), QDate::currentDate(), w);
        rec->setRange(1, 99999);
        form->addRow(tr("ID записи сборки"), rec);
        auto *btn = new QPushButton(tr("Завершить этап сборки"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [this, rec, end]() {
            QString err;
            if (!m_repo->callProcCompleteStage(rec->value(), end->date(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Этап завершён."));
            }
        });
        addProcGroup(tr("Завершить этап"), w);
    }

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
        auto *btn = new QPushButton(tr("Зарегистрировать испытание"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [this, inst, lab, date]() {
            QString err;
            if (!m_repo->callProcRegisterTest(inst->value(), lab->value(), date->date(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Испытание зарегистрировано."));
            }
        });
        addProcGroup(tr("Регистрация испытания"), w);
    }

    {
        auto *w = new QWidget(container);
        auto *form = new QFormLayout(w);
        auto *testId = new QSpinBox(w);
        auto *result = new QLineEdit(w);
        testId->setRange(1, 99999);
        form->addRow(tr("ID испытания"), testId);
        form->addRow(tr("Результат"), result);
        auto *btn = new QPushButton(tr("Завершить испытание"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [this, testId, result]() {
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
    return new ReportsHubWidget(m_repo, UserRole::Admin, this);
}
