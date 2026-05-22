#pragma once

#include "db/LookupItem.h"

#include <optional>

#include <QComboBox>
#include <QDateEdit>
#include <QFormLayout>
#include <QLayoutItem>
#include <QList>
#include <QSpinBox>

inline void clearFormLayout(QFormLayout *form)
{
    if (!form) {
        return;
    }
    while (form->rowCount() > 0) {
        const QFormLayout::TakeRowResult row = form->takeRow(0);
        if (row.labelItem) {
            if (QWidget *widget = row.labelItem->widget()) {
                delete widget;
            }
            delete row.labelItem;
        }
        if (row.fieldItem) {
            if (QWidget *widget = row.fieldItem->widget()) {
                delete widget;
            }
            delete row.fieldItem;
        }
    }
}

inline void fillEntityCombo(QComboBox *combo, const QList<LookupItem> &items, bool includeAll)
{
    combo->clear();
    if (includeAll) {
        combo->addItem(QObject::tr("Все"), QVariant());
    }
    for (const LookupItem &item : items) {
        combo->addItem(item.label, lookupItemData(item.id));
    }
    if (combo->count() > 0) {
        combo->setCurrentIndex(0);
    }
}

inline QComboBox *addOptionalEntityCombo(QFormLayout *form,
                                         const QString &label,
                                         QWidget *parent,
                                         const QList<LookupItem> &items)
{
    auto *combo = new QComboBox(parent);
    fillEntityCombo(combo, items, true);
    form->addRow(label, combo);
    return combo;
}

inline QComboBox *addRequiredEntityCombo(QFormLayout *form,
                                         const QString &label,
                                         QWidget *parent,
                                         const QList<LookupItem> &items)
{
    auto *combo = new QComboBox(parent);
    fillEntityCombo(combo, items, false);
    form->addRow(label, combo);
    return combo;
}

inline std::optional<int> optionalComboValue(const QComboBox *combo)
{
    if (!combo || combo->currentIndex() < 0) {
        return std::nullopt;
    }
    const QVariant data = combo->currentData();
    if (!data.isValid()) {
        return std::nullopt;
    }
    bool ok = false;
    const int value = data.toInt(&ok);
    if (!ok) {
        return std::nullopt;
    }
    return value;
}

inline std::optional<int> requireComboValue(const QComboBox *combo,
                                            const QString &label,
                                            QString *errorMessage)
{
    if (const std::optional<int> value = optionalComboValue(combo)) {
        return value;
    }
    if (errorMessage) {
        *errorMessage = QObject::tr("Выберите значение: %1").arg(label);
    }
    return std::nullopt;
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
