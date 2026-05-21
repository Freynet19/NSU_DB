#pragma once

#include <QString>
#include <QVariant>

struct LookupItem
{
    int id = 0;
    QString label;
};

inline QVariant lookupItemData(int id)
{
    return id;
}
