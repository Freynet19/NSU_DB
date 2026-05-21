#include "widgets/ReportTableWidget.h"

#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>
#include <QVBoxLayout>

ReportTableWidget::~ReportTableWidget()
{
    if (m_model) {
        m_view->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }
}

ReportTableWidget::ReportTableWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    m_filters = new QFormLayout;
    layout->addLayout(m_filters);

    auto *runButton = new QPushButton(tr("Выполнить запрос"), this);
    layout->addWidget(runButton);

    m_view = new QTableView(this);
    m_view->setAlternatingRowColors(true);
    m_view->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_view);

    connect(runButton, &QPushButton::clicked, this, &ReportTableWidget::runReport);
}

QFormLayout *ReportTableWidget::filterLayout() const
{
    return m_filters;
}

void ReportTableWidget::setRunReport(RunReportFn fn)
{
    m_runReport = std::move(fn);
}

void ReportTableWidget::runReport()
{
    if (!m_runReport) {
        return;
    }

    if (m_model) {
        m_view->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }

    QString error;
    m_model = m_runReport(&error);
    if (!m_model) {
        QMessageBox::critical(this, tr("Ошибка запроса"), error);
        return;
    }

    m_view->setModel(m_model);
    m_view->resizeColumnsToContents();
}
