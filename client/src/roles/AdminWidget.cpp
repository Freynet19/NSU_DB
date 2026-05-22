#include "roles/AdminWidget.h"

#include "core/UserRole.h"
#include "db/DatabaseManager.h"
#include "db/QueryRepository.h"
#include "widgets/CrudHubWidget.h"
#include "widgets/ProceduresHubWidget.h"
#include "widgets/ReportsHubWidget.h"

#include <QTabWidget>
#include <QVBoxLayout>

AdminWidget::AdminWidget(QWidget* parent)
    : QWidget(parent)
      , m_repo(new QueryRepository(DatabaseManager::instance().connectionName()))
{
    m_tabs = new QTabWidget(this);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_tabs);

    m_tabs->addTab(buildProceduresTab(), tr("Процедуры"));
    m_tabs->addTab(buildReportsTab(), tr("Отчёты"));
    m_tabs->addTab(new CrudHubWidget(this), tr("Справочники"));
}

QWidget* AdminWidget::buildProceduresTab()
{
    return new ProceduresHubWidget(m_repo, UserRole::Admin, this);
}

QWidget* AdminWidget::buildReportsTab()
{
    return new ReportsHubWidget(m_repo, UserRole::Admin, this);
}
