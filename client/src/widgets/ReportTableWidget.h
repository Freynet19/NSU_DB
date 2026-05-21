#pragma once

#include <functional>

#include <QWidget>

class QFormLayout;
class QSqlQueryModel;
class QTableView;

class ReportTableWidget : public QWidget
{
    Q_OBJECT

public:
    using RunReportFn = std::function<QSqlQueryModel *(QString *)>;

    explicit ReportTableWidget(const QString &title, QWidget *parent = nullptr);
    ~ReportTableWidget() override;

    QFormLayout *filterLayout() const;
    void setRunReport(RunReportFn fn);
    void runReport();

private:
    QString m_title;
    QFormLayout *m_filters = nullptr;
    QTableView *m_view = nullptr;
    QSqlQueryModel *m_model = nullptr;
    RunReportFn m_runReport;
};
