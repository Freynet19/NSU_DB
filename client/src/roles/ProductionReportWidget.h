#pragma once

#include <QWidget>

class QueryRepository;

class ProductionReportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProductionReportWidget(QWidget *parent = nullptr);

private:
    QueryRepository *m_repo = nullptr;
};
