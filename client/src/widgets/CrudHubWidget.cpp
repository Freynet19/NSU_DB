#include "widgets/CrudHubWidget.h"

#include "db/DatabaseManager.h"
#include "db/LookupRepository.h"
#include "ui_CrudHubWidget.h"
#include "widgets/TableCrudWidget.h"

#include <QBrush>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSignalBlocker>

CrudHubWidget::CrudHubWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CrudHubWidget)
{
    ui->setupUi(this);
    ui->rightLayout->setStretch(1, 1);

    populateTableList();

    connect(ui->tableList, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row < 0) {
            return;
        }
        if (!switchToTable(row)) {
            const QSignalBlocker blocker(ui->tableList);
            ui->tableList->setCurrentRow(m_currentListIndex);
        }
    });

    for (int row = 0; row < ui->tableList->count(); ++row) {
        if (m_listEntries.at(row).isGroupHeader) {
            continue;
        }
        ui->tableList->setCurrentRow(row);
        break;
    }
}

CrudHubWidget::~CrudHubWidget()
{
    delete ui;
}

void CrudHubWidget::populateTableList()
{
    m_listEntries.clear();
    ui->tableList->clear();

    CrudTableGroup currentGroup = CrudTableGroup::Reference;
    bool groupOpen = false;

    for (const CrudTableDefinition &table : CrudTableCatalog::tables()) {
        if (!groupOpen || table.group != currentGroup) {
            currentGroup = table.group;
            groupOpen = true;

            ListEntry header;
            header.isGroupHeader = true;
            header.group = currentGroup;
            m_listEntries.append(header);

            auto *headerItem = new QListWidgetItem(CrudTableCatalog::groupTitle(currentGroup), ui->tableList);
            headerItem->setFlags(Qt::NoItemFlags);
            headerItem->setForeground(QBrush(Qt::gray));
            QFont font = headerItem->font();
            font.setBold(true);
            headerItem->setFont(font);
        }

        ListEntry entry;
        entry.table = table;
        m_listEntries.append(entry);

        auto *item = new QListWidgetItem(table.displayName, ui->tableList);
        item->setToolTip(table.description);
        item->setData(Qt::UserRole, table.tableName);
    }
}

TableCrudWidget *CrudHubWidget::tableWidget(const QString &tableName)
{
    if (TableCrudWidget *existing = m_tableWidgets.value(tableName)) {
        return existing;
    }

    auto *widget = new TableCrudWidget(tableName, {}, this);
    connect(widget, &TableCrudWidget::dataCommitted, this, [this]() {
        LookupRepository(DatabaseManager::instance().connectionName()).clearCache();
    });
    m_tableWidgets.insert(tableName, widget);
    ui->crudStack->addWidget(widget);
    return widget;
}

bool CrudHubWidget::switchToTable(int listIndex)
{
    if (listIndex < 0 || listIndex >= m_listEntries.size()) {
        ui->descriptionLabel->clear();
        m_currentListIndex = -1;
        return true;
    }

    const ListEntry &entry = m_listEntries.at(listIndex);
    if (entry.isGroupHeader) {
        return false;
    }

    if (m_currentListIndex >= 0 && m_currentListIndex < m_listEntries.size()) {
        const ListEntry &current = m_listEntries.at(m_currentListIndex);
        if (!current.isGroupHeader) {
            if (TableCrudWidget *currentWidget = m_tableWidgets.value(current.table.tableName)) {
                if (!currentWidget->maybeDiscardChanges()) {
                    return false;
                }
            }
        }
    }

    TableCrudWidget *widget = tableWidget(entry.table.tableName);
    ui->crudStack->setCurrentWidget(widget);
    ui->descriptionLabel->setText(entry.table.description);
    m_currentListIndex = listIndex;
    return true;
}
