#include "widgets/TableCrudWidget.h"

#include "crud/CrudTableCatalog.h"
#include "db/DatabaseManager.h"
#include "ui_TableCrudWidget.h"
#include "widgets/FilterHelpers.h"

#include <QFont>
#include <QHeaderView>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlTableModel>

namespace
{
    class CrudSqlTableModel : public QSqlTableModel
    {
    public:
        CrudSqlTableModel(const QString& tableName, QObject* parent, QSqlDatabase db)
            : QSqlTableModel(parent, db)
              , m_tableName(tableName)
        {
        }

        Qt::ItemFlags flags(const QModelIndex& index) const override
        {
            Qt::ItemFlags itemFlags = QSqlTableModel::flags(index);
            if (!index.isValid())
            {
                return itemFlags;
            }
            const QString fieldName = record().fieldName(index.column());
            if (CrudTableCatalog::isReadOnlyColumn(m_tableName, fieldName))
            {
                itemFlags &= ~Qt::ItemIsEditable;
            }
            return itemFlags;
        }

    private:
        QString m_tableName;
    };
} // namespace

TableCrudWidget::TableCrudWidget(const QString& tableName,
                                 const QString& filterClause,
                                 QWidget* parent)
    : QWidget(parent)
      , m_tableName(tableName)
      , m_filterClause(filterClause)
      , ui(new Ui::TableCrudWidget)
{
    ui->setupUi(this);

    QFont titleFont = ui->titleLabel->font();
    titleFont.setBold(true);
    ui->titleLabel->setFont(titleFont);
    ui->titleLabel->setText(
        tr("%1 — редактирование справочника").arg(CrudTableCatalog::tableDisplayName(m_tableName)));

    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ui->saveButton->setEnabled(false);
    ui->revertButton->setEnabled(false);

    connect(ui->addButton, &QPushButton::clicked, this, &TableCrudWidget::addRow);
    connect(ui->removeButton, &QPushButton::clicked, this, &TableCrudWidget::removeRow);
    connect(ui->saveButton, &QPushButton::clicked, this, &TableCrudWidget::saveChanges);
    connect(ui->revertButton, &QPushButton::clicked, this, &TableCrudWidget::revertChanges);
    connect(ui->refreshButton, &QPushButton::clicked, this, &TableCrudWidget::onRefreshClicked);

    refresh();
}

TableCrudWidget::~TableCrudWidget()
{
    if (m_model)
    {
        ui->tableView->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }
    delete ui;
}

QString TableCrudWidget::tableName() const
{
    return m_tableName;
}

bool TableCrudWidget::hasUnsavedChanges() const
{
    return m_dirty;
}

void TableCrudWidget::connectModelSignals()
{
    if (!m_model)
    {
        return;
    }

    connect(m_model, &QSqlTableModel::dataChanged, this, [this]() { setDirty(true); });
    connect(m_model, &QSqlTableModel::rowsInserted, this, [this]() { setDirty(true); });
    connect(m_model, &QSqlTableModel::rowsRemoved, this, [this]() { setDirty(true); });
    connect(m_model, &QSqlTableModel::modelReset, this, &TableCrudWidget::updateStatus);
}

void TableCrudWidget::setDirty(bool dirty)
{
    m_dirty = dirty;
    ui->saveButton->setEnabled(dirty);
    ui->revertButton->setEnabled(dirty);
    updateStatus();
}

void TableCrudWidget::applyColumnHeaders()
{
    if (!m_model)
    {
        return;
    }
    for (int col = 0; col < m_model->columnCount(); ++col)
    {
        const QString fieldName = m_model->record().fieldName(col);
        m_model->setHeaderData(col, Qt::Horizontal,
                               CrudTableCatalog::columnTitle(m_tableName, fieldName));
    }
}

void TableCrudWidget::updateStatus()
{
    if (!m_model)
    {
        ui->statusLabel->setText(tr("Ошибка загрузки таблицы"));
        return;
    }

    const int rows = m_model->rowCount();
    QString text;
    if (rows == 0)
    {
        text = tr("Нет записей. Нажмите «Добавить», чтобы создать строку.");
    }
    else
    {
        text = tr("Строк: %1").arg(rows);
    }

    if (m_dirty)
    {
        text += tr(" · Есть несохранённые изменения");
    }

    ui->statusLabel->setText(text);
}

void TableCrudWidget::refresh()
{
    const int previousRow = ui->tableView->currentIndex().isValid()
                                ? ui->tableView->currentIndex().row()
                                : -1;

    if (m_model)
    {
        ui->tableView->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }

    const QString connectionName = DatabaseManager::instance().connectionName();
    m_model = new CrudSqlTableModel(m_tableName, this, QSqlDatabase::database(connectionName));
    m_model->setTable(m_tableName);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    if (!m_filterClause.isEmpty())
    {
        m_model->setFilter(m_filterClause);
    }

    if (!m_model->select())
    {
        QMessageBox::critical(this, tr("Ошибка"), m_model->lastError().text());
        setDirty(false);
        updateStatus();
        return;
    }

    applyColumnHeaders();
    ui->tableView->setModel(m_model);
    ui->tableView->resizeColumnsToContents();

    if (previousRow >= 0 && previousRow < m_model->rowCount())
    {
        ui->tableView->selectRow(previousRow);
    }

    connectModelSignals();
    setDirty(false);
    updateStatus();
}

bool TableCrudWidget::maybeDiscardChanges()
{
    if (!m_dirty)
    {
        return true;
    }

    const QMessageBox::StandardButton answer = QMessageBox::question(
        this,
        tr("Несохранённые изменения"),
        tr("В таблице «%1» есть несохранённые изменения. Отменить их и перейти дальше?")
        .arg(CrudTableCatalog::tableDisplayName(m_tableName)),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (answer != QMessageBox::Yes)
    {
        return false;
    }

    revertChanges();
    return true;
}

void TableCrudWidget::onRefreshClicked()
{
    if (!maybeDiscardChanges())
    {
        return;
    }
    refresh();
}

void TableCrudWidget::addRow()
{
    if (!m_model)
    {
        return;
    }
    const int row = m_model->rowCount();
    m_model->insertRow(row);
    ui->tableView->selectRow(row);
    setDirty(true);
}

void TableCrudWidget::removeRow()
{
    if (!m_model)
    {
        return;
    }
    const QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::information(this, tr("Удаление"), tr("Выберите строку для удаления."));
        return;
    }

    const QMessageBox::StandardButton answer = QMessageBox::question(
        this,
        tr("Удаление"),
        tr("Удалить выбранную строку? Изменения нужно сохранить кнопкой «Сохранить»."),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (answer != QMessageBox::Yes)
    {
        return;
    }

    m_model->removeRow(index.row());
    setDirty(true);
}

void TableCrudWidget::saveChanges()
{
    if (!m_model)
    {
        return;
    }

    for (int row = 0; row < m_model->rowCount(); ++row)
    {
        for (int col = 0; col < m_model->columnCount(); ++col)
        {
            const QString fieldName = m_model->record().fieldName(col);
            if (!CrudTableCatalog::requiresNonEmptyString(m_tableName, fieldName))
            {
                continue;
            }
            const QString value = m_model->data(m_model->index(row, col)).toString();
            if (!isBlankText(value))
            {
                continue;
            }
            QMessageBox::warning(
                this,
                tr("Проверка данных"),
                tr("Строка %1: заполните поле «%2».")
                .arg(row + 1)
                .arg(CrudTableCatalog::columnTitle(m_tableName, fieldName)));
            return;
        }
    }

    const int currentRow = ui->tableView->currentIndex().row();

    QSignalBlocker blocker(m_model);

    if (!m_model->submitAll())
    {
        QMessageBox::critical(this, tr("Ошибка сохранения"), m_model->lastError().text());
        return;
    }

    if (!m_model->select())
    {
        QMessageBox::critical(this, tr("Ошибка"), m_model->lastError().text());
        return;
    }

    applyColumnHeaders();

    if (currentRow >= 0 && currentRow < m_model->rowCount())
    {
        ui->tableView->selectRow(currentRow);
    }

    setDirty(false);
    updateStatus();
    emit dataCommitted();
}

void TableCrudWidget::revertChanges()
{
    if (m_model)
    {
        m_model->revertAll();
    }
    setDirty(false);
    updateStatus();
}
