#include "widgets/ProceduresHubWidget.h"

#include "db/DatabaseManager.h"
#include "db/LookupRepository.h"
#include "db/QueryRepository.h"
#include "procedures/ProcedureCatalog.h"
#include "procedures/ProcedureFormBuilder.h"
#include "ui_ProceduresHubWidget.h"
#include "widgets/ProcedureFormWidget.h"

#include <QListWidget>
#include <QVBoxLayout>

ProceduresHubWidget::ProceduresHubWidget(QueryRepository *repository, UserRole role, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProceduresHubWidget)
    , m_repository(repository)
    , m_lookups(new LookupRepository(DatabaseManager::instance().connectionName()))
    , m_procedures(ProcedureCatalog::proceduresForRole(role))
{
    ui->setupUi(this);
    ui->rightLayout->setStretch(1, 1);

    auto *formLayout = new QVBoxLayout(ui->formHost);
    formLayout->setContentsMargins(0, 0, 0, 0);
    m_formWidget = new ProcedureFormWidget(ui->formHost);
    formLayout->addWidget(m_formWidget);

    for (const ProcedureDefinition &procedure : m_procedures) {
        auto *item =
            new QListWidgetItem(ProcedureCatalog::listItemTitle(procedure), ui->procedureList);
        item->setToolTip(QStringLiteral("%1\n(%2)")
                             .arg(procedure.description, procedure.sqlName));
        item->setData(Qt::UserRole, static_cast<int>(procedure.id));
    }

    connect(ui->procedureList, &QListWidget::currentRowChanged, this,
            &ProceduresHubWidget::onProcedureSelected);

    if (!m_procedures.isEmpty()) {
        ui->procedureList->setCurrentRow(0);
    }
}

ProceduresHubWidget::~ProceduresHubWidget()
{
    delete m_lookups;
    delete ui;
}

void ProceduresHubWidget::onProcedureSelected(int index)
{
    if (index < 0 || index >= m_procedures.size()) {
        ui->descriptionLabel->clear();
        m_formWidget->clearForm();
        return;
    }

    const ProcedureDefinition &procedure = m_procedures.at(index);
    ui->descriptionLabel->setText(
        QStringLiteral("%1 (%2)").arg(procedure.description, procedure.sqlName));
    ProcedureFormBuilder::configure(m_formWidget, procedure.id, m_repository, m_lookups, this);
}
