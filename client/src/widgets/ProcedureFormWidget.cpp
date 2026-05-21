#include "widgets/ProcedureFormWidget.h"

#include "widgets/FilterHelpers.h"

#include <QFormLayout>
#include <QVBoxLayout>

ProcedureFormWidget::ProcedureFormWidget(QWidget *parent)
    : QWidget(parent)
    , m_form(new QFormLayout)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(m_form);
    layout->addStretch();
}

QFormLayout *ProcedureFormWidget::formLayout() const
{
    return m_form;
}

void ProcedureFormWidget::clearForm()
{
    clearFormLayout(m_form);
}
