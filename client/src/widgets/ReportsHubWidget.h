#pragma once

#include "core/UserRole.h"
#include "reports/ReportCatalog.h"

#include <QList>
#include <QWidget>

class LookupRepository;
class QueryRepository;

namespace Ui {
class ReportsHubWidget;
}

class ReportsHubWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsHubWidget(QueryRepository *repository, UserRole role, QWidget *parent = nullptr);
    ~ReportsHubWidget() override;

private:
    void onReportSelected(int index);

    Ui::ReportsHubWidget *ui = nullptr;
    QueryRepository *m_repository = nullptr;
    LookupRepository *m_lookups = nullptr;
    QList<ReportDefinition> m_reports;
    class ReportTableWidget *m_reportWidget = nullptr;
};
