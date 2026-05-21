#pragma once

#include <QWidget>

class QSqlTableModel;
class QTableView;

class TableCrudWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TableCrudWidget(const QString &tableName,
                             const QString &filterClause = {},
                             QWidget *parent = nullptr);

    void refresh();

private slots:
    void addRow();
    void removeRow();
    void saveChanges();
    void revertChanges();

private:
    QString m_tableName;
    QString m_filterClause;
    QSqlTableModel *m_model = nullptr;
    QTableView *m_view = nullptr;
};
