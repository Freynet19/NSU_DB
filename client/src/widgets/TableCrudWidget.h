#pragma once

#include <QWidget>

class QSqlTableModel;

namespace Ui {
class TableCrudWidget;
}

class TableCrudWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TableCrudWidget(const QString &tableName,
                             const QString &filterClause = {},
                             QWidget *parent = nullptr);
    ~TableCrudWidget() override;

    QString tableName() const;
    bool hasUnsavedChanges() const;

    void refresh();

signals:
    void dataCommitted();

public slots:
    bool maybeDiscardChanges();

private slots:
    void addRow();
    void removeRow();
    void saveChanges();
    void revertChanges();
    void onRefreshClicked();
    void updateStatus();

private:
    void applyColumnHeaders();
    void connectModelSignals();
    void setDirty(bool dirty);

    QString m_tableName;
    QString m_filterClause;
    QSqlTableModel *m_model = nullptr;
    bool m_dirty = false;
    Ui::TableCrudWidget *ui = nullptr;
};
