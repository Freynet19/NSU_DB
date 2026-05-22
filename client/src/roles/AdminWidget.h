#pragma once

#include <QWidget>

class QueryRepository;
class QTabWidget;

class AdminWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWidget(QWidget* parent = nullptr);

private:
    QWidget* buildProceduresTab();
    QWidget* buildReportsTab();
    QTabWidget* m_tabs = nullptr;
    QueryRepository* m_repo = nullptr;
};
