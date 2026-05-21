#pragma once

#include <functional>

#include <QWidget>

class QFormLayout;
class QSqlQueryModel;
class QTableView;

namespace Ui {
class ReportTableWidget;
}

class ReportTableWidget : public QWidget
{
    Q_OBJECT

public:
    using RunReportFn = std::function<QSqlQueryModel *(QString *)>;

    explicit ReportTableWidget(QWidget *parent = nullptr);
    ~ReportTableWidget() override;

    QFormLayout *filterLayout() const;
    void clearFilters();
    void clearResults();
    void setRunReport(RunReportFn fn);
    void setRunCount(RunReportFn fn);
    void setCountCaption(const QString &caption);
    void runReport();

private:
    void updateCountDisplay();
    void hideCountDisplay();

    Ui::ReportTableWidget *ui = nullptr;
    QSqlQueryModel *m_model = nullptr;
    RunReportFn m_runReport;
    RunReportFn m_runCount;
    QString m_countCaption;
};
