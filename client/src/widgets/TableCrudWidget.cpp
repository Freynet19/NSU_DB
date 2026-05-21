#include "widgets/TableCrudWidget.h"

#include "db/DatabaseManager.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlTableModel>
#include <QTableView>
#include <QVBoxLayout>

TableCrudWidget::TableCrudWidget(const QString &tableName,
                                 const QString &filterClause,
                                 QWidget *parent)
    : QWidget(parent)
    , m_tableName(tableName)
    , m_filterClause(filterClause)
{
    auto *layout = new QVBoxLayout(this);

    auto *toolbar = new QHBoxLayout;
    auto *addButton = new QPushButton(tr("Добавить"), this);
    auto *removeButton = new QPushButton(tr("Удалить"), this);
    auto *saveButton = new QPushButton(tr("Сохранить"), this);
    auto *revertButton = new QPushButton(tr("Отменить"), this);
    auto *refreshButton = new QPushButton(tr("Обновить"), this);

    toolbar->addWidget(addButton);
    toolbar->addWidget(removeButton);
    toolbar->addWidget(saveButton);
    toolbar->addWidget(revertButton);
    toolbar->addWidget(refreshButton);
    toolbar->addStretch();
    layout->addLayout(toolbar);

    m_view = new QTableView(this);
    m_view->setAlternatingRowColors(true);
    m_view->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_view);

    connect(addButton, &QPushButton::clicked, this, &TableCrudWidget::addRow);
    connect(removeButton, &QPushButton::clicked, this, &TableCrudWidget::removeRow);
    connect(saveButton, &QPushButton::clicked, this, &TableCrudWidget::saveChanges);
    connect(revertButton, &QPushButton::clicked, this, &TableCrudWidget::revertChanges);
    connect(refreshButton, &QPushButton::clicked, this, &TableCrudWidget::refresh);

    refresh();
}

void TableCrudWidget::refresh()
{
    if (m_model) {
        m_view->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }

    m_model = new QSqlTableModel(this, DatabaseManager::instance().database());
    m_model->setTable(m_tableName);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    if (!m_filterClause.isEmpty()) {
        m_model->setFilter(m_filterClause);
    }

    if (!m_model->select()) {
        QMessageBox::critical(this, tr("Ошибка"), m_model->lastError().text());
        return;
    }

    m_view->setModel(m_model);
    m_view->resizeColumnsToContents();
}

void TableCrudWidget::addRow()
{
    if (!m_model) {
        return;
    }
    const int row = m_model->rowCount();
    m_model->insertRow(row);
}

void TableCrudWidget::removeRow()
{
    if (!m_model) {
        return;
    }
    const QModelIndex index = m_view->currentIndex();
    if (!index.isValid()) {
        return;
    }
    m_model->removeRow(index.row());
}

void TableCrudWidget::saveChanges()
{
    if (!m_model) {
        return;
    }
    if (!m_model->submitAll()) {
        QMessageBox::critical(this, tr("Ошибка сохранения"), m_model->lastError().text());
        return;
    }
    refresh();
}

void TableCrudWidget::revertChanges()
{
    if (m_model) {
        m_model->revertAll();
    }
}
