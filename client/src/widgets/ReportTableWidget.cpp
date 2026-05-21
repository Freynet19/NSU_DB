#include "widgets/ReportTableWidget.h"

#include "widgets/FilterHelpers.h"
#include "ui_ReportTableWidget.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlQueryModel>

ReportTableWidget::ReportTableWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReportTableWidget)
{
    ui->setupUi(this);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    connect(ui->runButton, &QPushButton::clicked, this, &ReportTableWidget::runReport);
}

ReportTableWidget::~ReportTableWidget()
{
    if (m_model) {
        ui->tableView->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }
    delete ui;
}

QFormLayout *ReportTableWidget::filterLayout() const
{
    return ui->filterLayout;
}

void ReportTableWidget::clearFilters()
{
    clearFormLayout(ui->filterLayout);
    m_runReport = {};
}

void ReportTableWidget::clearResults()
{
    if (m_model) {
        ui->tableView->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }
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
        ui->tableView->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }

    QString error;
    m_model = m_runReport(&error);
    if (!m_model) {
        QMessageBox::critical(this, tr("Ошибка запроса"), error);
        return;
    }

    ui->tableView->setModel(m_model);
    ui->tableView->resizeColumnsToContents();
}
