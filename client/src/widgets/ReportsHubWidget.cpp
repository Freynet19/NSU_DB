#include "widgets/ReportsHubWidget.h"

#include "db/DatabaseManager.h"
#include "db/LookupRepository.h"
#include "db/QueryRepository.h"
#include "reports/ReportCatalog.h"
#include "reports/ReportFilterBuilder.h"
#include "ui_ReportsHubWidget.h"
#include "widgets/ReportTableWidget.h"

#include <QListWidget>
#include <QVBoxLayout>

ReportsHubWidget::ReportsHubWidget(QueryRepository *repository, UserRole role, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReportsHubWidget)
    , m_repository(repository)
    , m_lookups(new LookupRepository(DatabaseManager::instance().connectionName()))
    , m_reports(ReportCatalog::reportsForRole(role))
{
    ui->setupUi(this);

    auto *reportLayout = new QVBoxLayout(ui->reportHost);
    reportLayout->setContentsMargins(0, 0, 0, 0);
    m_reportWidget = new ReportTableWidget(ui->reportHost);
    reportLayout->addWidget(m_reportWidget);

    for (const ReportDefinition &report : m_reports) {
        auto *item = new QListWidgetItem(ReportCatalog::listItemTitle(report), ui->reportList);
        item->setToolTip(report.description);
        item->setData(Qt::UserRole, static_cast<int>(report.id));
    }

    connect(ui->reportList, &QListWidget::currentRowChanged, this, &ReportsHubWidget::onReportSelected);

    if (!m_reports.isEmpty()) {
        ui->reportList->setCurrentRow(0);
    }
}

ReportsHubWidget::~ReportsHubWidget()
{
    delete m_lookups;
    delete ui;
}

void ReportsHubWidget::onReportSelected(int index)
{
    if (index < 0 || index >= m_reports.size()) {
        ui->descriptionLabel->clear();
        m_reportWidget->clearFilters();
        m_reportWidget->clearResults();
        return;
    }

    const ReportDefinition &report = m_reports.at(index);
    ui->descriptionLabel->setText(report.description);
    m_reportWidget->clearResults();
    ReportFilterBuilder::configure(m_reportWidget, report.id, m_repository, m_lookups);
}
