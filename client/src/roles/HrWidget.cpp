#include "roles/HrWidget.h"

#include "core/UserRole.h"
#include "db/DatabaseManager.h"
#include "db/QueryRepository.h"
#include "widgets/ProceduresHubWidget.h"
#include "widgets/ReportsHubWidget.h"

#include <QTabWidget>
#include <QVBoxLayout>

HrWidget::HrWidget(QWidget* parent)
    : QWidget(parent)
      , m_repo(new QueryRepository(DatabaseManager::instance().connectionName()))
{
    auto* tabs = new QTabWidget(this);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(tabs);

    tabs->addTab(new ProceduresHubWidget(m_repo, UserRole::Hr, this), tr("Кадровые операции"));
    tabs->addTab(new ReportsHubWidget(m_repo, UserRole::Hr, this), tr("Отчёты"));
}
