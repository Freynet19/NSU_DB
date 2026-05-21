#pragma once

#include <optional>

#include <QComboBox>
#include <QDateEdit>
#include <QFormLayout>
#include <QLayoutItem>
#include <QSpinBox>

inline void clearFormLayout(QFormLayout *form)
{
    if (!form) {
        return;
    }
    while (QLayoutItem *item = form->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

inline QSpinBox *addOptionalIntFilter(QFormLayout *form, const QString &label, QWidget *parent)
{
    auto *spin = new QSpinBox(parent);
    spin->setMinimum(-1);
    spin->setMaximum(999999);
    spin->setSpecialValueText(QObject::tr("Все"));
    spin->setValue(-1);
    form->addRow(label, spin);
    return spin;
}

inline std::optional<int> optionalSpinValue(const QSpinBox *spin)
{
    if (!spin || spin->value() < 0) {
        return std::nullopt;
    }
    return spin->value();
}

inline QDateEdit *addDateFilter(QFormLayout *form, const QString &label, const QDate &date, QWidget *parent)
{
    auto *edit = new QDateEdit(date, parent);
    edit->setCalendarPopup(true);
    edit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    form->addRow(label, edit);
    return edit;
}

inline QComboBox *addPersonnelTypeFilter(QFormLayout *form, QWidget *parent)
{
    auto *combo = new QComboBox(parent);
    combo->addItem(QObject::tr("Все"), QString());
    combo->addItem(QStringLiteral("ITP"), QStringLiteral("ITP"));
    combo->addItem(QStringLiteral("Worker"), QStringLiteral("Worker"));
    form->addRow(QObject::tr("Тип персонала"), combo);
    return combo;
}
