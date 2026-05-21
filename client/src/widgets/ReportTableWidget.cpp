#include "widgets/ReportTableWidget.h"

#include "widgets/FilterHelpers.h"
#include "ui_ReportTableWidget.h"

#include <optional>

#include <QFont>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlQueryModel>

namespace {

std::optional<qlonglong> scalarCountFromModel(QSqlQueryModel *model)
{
    if (!model || model->rowCount() == 0 || model->columnCount() == 0) {
        return std::nullopt;
    }
    bool ok = false;
    const qlonglong value = model->data(model->index(0, 0)).toLongLong(&ok);
    if (!ok) {
        return std::nullopt;
    }
    return value;
}

} // namespace

ReportTableWidget::ReportTableWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReportTableWidget)
{
    ui->setupUi(this);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    hideCountDisplay();
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
    m_runCount = {};
    m_countCaption.clear();
}

void ReportTableWidget::clearResults()
{
    if (m_model) {
        ui->tableView->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }
    hideCountDisplay();
}

void ReportTableWidget::setRunReport(RunReportFn fn)
{
    m_runReport = std::move(fn);
}

void ReportTableWidget::setRunCount(RunReportFn fn)
{
    m_runCount = std::move(fn);
}

void ReportTableWidget::setCountCaption(const QString &caption)
{
    m_countCaption = caption;
}

void ReportTableWidget::hideCountDisplay()
{
    ui->countLabel->clear();
    ui->countLabel->hide();
}

void ReportTableWidget::updateCountDisplay()
{
    if (!m_runCount || m_countCaption.isEmpty()) {
        hideCountDisplay();
        return;
    }

    QString error;
    QSqlQueryModel *countModel = m_runCount(&error);
    if (!countModel) {
        hideCountDisplay();
        if (!error.isEmpty()) {
            QMessageBox::critical(this, tr("Ошибка запроса"), error);
        }
        return;
    }

    const std::optional<qlonglong> count = scalarCountFromModel(countModel);
    delete countModel;

    if (!count.has_value()) {
        hideCountDisplay();
        return;
    }

    QFont font = ui->countLabel->font();
    font.setBold(true);
    ui->countLabel->setFont(font);
    ui->countLabel->setText(tr("%1: %2").arg(m_countCaption).arg(*count));
    ui->countLabel->show();
}

void ReportTableWidget::runReport()
{
    if (!m_runReport && !m_runCount) {
        return;
    }

    if (m_model) {
        ui->tableView->setModel(nullptr);
        delete m_model;
        m_model = nullptr;
    }
    hideCountDisplay();

    if (m_runCount) {
        updateCountDisplay();
    }

    if (!m_runReport) {
        return;
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
