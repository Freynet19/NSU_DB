#pragma once

#include "core/UserRole.h"
#include "procedures/ProcedureCatalog.h"

#include <QList>
#include <QWidget>

class QueryRepository;

namespace Ui {
class ProceduresHubWidget;
}

class ProceduresHubWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProceduresHubWidget(QueryRepository *repository, UserRole role, QWidget *parent = nullptr);
    ~ProceduresHubWidget() override;

private:
    void onProcedureSelected(int index);

    Ui::ProceduresHubWidget *ui = nullptr;
    QueryRepository *m_repository = nullptr;
    QList<ProcedureDefinition> m_procedures;
    class ProcedureFormWidget *m_formWidget = nullptr;
};
