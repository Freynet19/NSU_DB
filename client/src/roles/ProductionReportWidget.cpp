#include "roles/ProductionReportWidget.h"

#include "core/UserRole.h"
#include "db/DatabaseManager.h"
#include "db/QueryRepository.h"
#include "widgets/ReportsHubWidget.h"

#include <QVBoxLayout>

ProductionReportWidget::ProductionReportWidget(QWidget *parent)
    : QWidget(parent)
    , m_repo(new QueryRepository(DatabaseManager::instance().connectionName()))
{
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new ReportsHubWidget(m_repo, UserRole::ProductionReport, this));
}
