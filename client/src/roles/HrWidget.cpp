#include "roles/HrWidget.h"

#include "db/DatabaseManager.h"
#include "db/QueryRepository.h"
#include "widgets/FilterHelpers.h"
#include "widgets/ReportTableWidget.h"

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

HrWidget::HrWidget(QWidget *parent)
    : QWidget(parent)
    , m_repo(new QueryRepository(DatabaseManager::instance().connectionName()))
{
    auto *tabs = new QTabWidget(this);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(tabs);

    // Operations tab
    auto *opsScroll = new QScrollArea(this);
    opsScroll->setWidgetResizable(true);
    auto *ops = new QWidget(opsScroll);
    auto *opsLayout = new QVBoxLayout(ops);

    auto addGroup = [&](const QString &title, QWidget *content) {
        auto *box = new QGroupBox(title, ops);
        auto *boxLayout = new QVBoxLayout(box);
        boxLayout->addWidget(content);
        opsLayout->addWidget(box);
    };

    {
        auto *w = new QWidget(ops);
        auto *form = new QFormLayout(w);
        auto *name = new QLineEdit(w);
        auto *birth = addDateFilter(form, tr("Дата рождения"), QDate(1995, 3, 15), w);
        auto *hire = addDateFilter(form, tr("Дата приёма"), QDate::currentDate(), w);
        auto *cat = new QSpinBox(w);
        cat->setRange(1, 8);
        cat->setValue(1);
        auto *spec = new QLineEdit(QStringLiteral("Слесарь-сборщик"), w);
        auto *grade = new QSpinBox(w);
        grade->setRange(1, 8);
        grade->setValue(4);
        auto *brigade = addOptionalIntFilter(form, tr("Бригада"), w);
        form->addRow(tr("ФИО"), name);
        form->addRow(tr("Код категории"), cat);
        form->addRow(tr("Специальность"), spec);
        form->addRow(tr("Разряд"), grade);
        auto *btn = new QPushButton(tr("Принять рабочего (proc_hire_worker)"), w);
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
        addGroup(tr("Приём рабочего"), w);
    }

    {
        auto *w = new QWidget(ops);
        auto *form = new QFormLayout(w);
        auto *name = new QLineEdit(w);
        auto *birth = addDateFilter(form, tr("Дата рождения"), QDate(1988, 7, 20), w);
        auto *hire = addDateFilter(form, tr("Дата приёма"), QDate::currentDate(), w);
        auto *cat = new QSpinBox(w);
        cat->setRange(6, 8);
        cat->setValue(7);
        auto *pos = new QLineEdit(QStringLiteral("Мастер участка"), w);
        auto *qual = new QLineEdit(w);
        form->addRow(tr("ФИО"), name);
        form->addRow(tr("Код категории"), cat);
        form->addRow(tr("Должность"), pos);
        form->addRow(tr("Квалификация"), qual);
        auto *btn = new QPushButton(tr("Принять ИТП (proc_hire_itp)"), w);
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
        addGroup(tr("Приём ИТП"), w);
    }

    {
        auto *w = new QWidget(ops);
        auto *form = new QFormLayout(w);
        auto *emp = new QSpinBox(w);
        emp->setRange(1, 99999);
        auto *brigade = addOptionalIntFilter(form, tr("Новая бригада"), w);
        auto *section = addOptionalIntFilter(form, tr("Участок мастера"), w);
        auto *desc = new QLineEdit(w);
        form->addRow(tr("ID сотрудника"), emp);
        form->addRow(tr("Описание"), desc);
        auto *btn = new QPushButton(tr("Перевод (proc_transfer_employee)"), w);
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
        addGroup(tr("Перевод"), w);
    }

    {
        auto *w = new QWidget(ops);
        auto *form = new QFormLayout(w);
        auto *emp = new QSpinBox(w);
        emp->setRange(1, 99999);
        auto *desc = new QLineEdit(w);
        form->addRow(tr("ID сотрудника"), emp);
        form->addRow(tr("Описание"), desc);
        auto *btn = new QPushButton(tr("Увольнение (proc_dismiss_employee)"), w);
        form->addRow(btn);
        connect(btn, &QPushButton::clicked, w, [=]() {
            QString err;
            if (!m_repo->callProcDismissEmployee(emp->value(), desc->text(), &err)) {
                QMessageBox::critical(this, tr("Ошибка"), err);
            } else {
                QMessageBox::information(this, tr("Готово"), tr("Увольнение зарегистрировано."));
            }
        });
        addGroup(tr("Увольнение"), w);
    }

    opsLayout->addStretch();
    opsScroll->setWidget(ops);
    tabs->addTab(opsScroll, tr("Кадровые операции"));

    // Reports 3,4,6,7 via VIEW / PREPARE
    auto *reports = new QTabWidget(this);

    {
        auto *r = new ReportTableWidget(tr("3. Кадровый состав"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        auto *type = addPersonnelTypeFilter(r->filterLayout(), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query3Personnel(optionalSpinValue(ws), optionalSpinValue(sec),
                                           type->currentData().toString(), err);
        });
        reports->addTab(r, tr("3"));
    }

    {
        auto *r = new ReportTableWidget(tr("4. Участки"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query4SectionList(optionalSpinValue(ws), err);
        });
        reports->addTab(r, tr("4"));
    }

    {
        auto *r = new ReportTableWidget(tr("6. Состав бригад"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query6BrigadeComposition(optionalSpinValue(ws), optionalSpinValue(sec), err);
        });
        reports->addTab(r, tr("6"));
    }

    {
        auto *r = new ReportTableWidget(tr("7. Мастера"), this);
        auto *ws = addOptionalIntFilter(r->filterLayout(), tr("Цех"), r);
        auto *sec = addOptionalIntFilter(r->filterLayout(), tr("Участок"), r);
        r->setRunReport([=](QString *err) {
            return m_repo->query7SectionMasters(optionalSpinValue(ws), optionalSpinValue(sec), err);
        });
        reports->addTab(r, tr("7"));
    }

    tabs->addTab(reports, tr("Отчёты HR"));
}
