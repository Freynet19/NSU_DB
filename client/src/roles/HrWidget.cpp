#include "roles/HrWidget.h"

#include "core/UserRole.h"
#include "db/DatabaseManager.h"
#include "db/QueryRepository.h"
#include "widgets/FilterHelpers.h"
#include "widgets/ReportsHubWidget.h"

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
        addGroup(tr("Увольнение"), w);
    }

    opsLayout->addStretch();
    opsScroll->setWidget(ops);
    tabs->addTab(opsScroll, tr("Кадровые операции"));
    tabs->addTab(new ReportsHubWidget(m_repo, UserRole::Hr, this), tr("Отчёты"));
}
