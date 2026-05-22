#pragma once

#include <QHash>
#include <QVector>
#include <QWidget>

#include "crud/CrudTableCatalog.h"

namespace Ui
{
    class CrudHubWidget;
}

class TableCrudWidget;

class CrudHubWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CrudHubWidget(QWidget* parent = nullptr);
    ~CrudHubWidget() override;

private:
    struct ListEntry
    {
        bool isGroupHeader = false;
        CrudTableGroup group = CrudTableGroup::Reference;
        CrudTableDefinition table;
    };

    void populateTableList();
    bool switchToTable(int listIndex);
    TableCrudWidget* tableWidget(const QString& tableName);

    Ui::CrudHubWidget* ui = nullptr;
    QVector<ListEntry> m_listEntries;
    QHash<QString, TableCrudWidget*> m_tableWidgets;
    int m_currentListIndex = -1;
};
