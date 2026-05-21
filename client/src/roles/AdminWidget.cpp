#include "roles/AdminWidget.h"

#include "core/UserRole.h"
#include "db/DatabaseManager.h"
#include "db/QueryRepository.h"
#include "widgets/ProceduresHubWidget.h"
#include "widgets/ReportsHubWidget.h"
#include "widgets/TableCrudWidget.h"

#include <QMap>
#include <QTabWidget>
#include <QVBoxLayout>

namespace {

QString tableDisplayName(const QString &table)
{
    static const QMap<QString, QString> names = {
        {QStringLiteral("workshop"), QStringLiteral("Цех")},
        {QStringLiteral("section"), QStringLiteral("Участок")},
        {QStringLiteral("brigade"), QStringLiteral("Бригада")},
        {QStringLiteral("personnel_category"), QStringLiteral("Категория персонала")},
        {QStringLiteral("product_category"), QStringLiteral("Категория изделий")},
        {QStringLiteral("work_type"), QStringLiteral("Вид работ")},
        {QStringLiteral("laboratory"), QStringLiteral("Лаборатория")},
        {QStringLiteral("equipment"), QStringLiteral("Оборудование")},
        {QStringLiteral("employee"), QStringLiteral("Сотрудник")},
        {QStringLiteral("worker"), QStringLiteral("Рабочий")},
        {QStringLiteral("itp"), QStringLiteral("ИТП")},
        {QStringLiteral("product_type"), QStringLiteral("Вид изделия")},
        {QStringLiteral("product_instance"), QStringLiteral("Экземпляр изделия")},
        {QStringLiteral("production_cycle"), QStringLiteral("Производственный цикл")},
        {QStringLiteral("assembly_record"), QStringLiteral("Запись сборки")},
        {QStringLiteral("test"), QStringLiteral("Испытание")},
        {QStringLiteral("test_specialist"), QStringLiteral("Испытатель")},
        {QStringLiteral("test_equipment"), QStringLiteral("Оборудование испытания")},
        {QStringLiteral("workshop_laboratory"), QStringLiteral("Лаборатория цеха")},
        {QStringLiteral("personnel_movement"), QStringLiteral("Кадровое движение")},
    };
    return names.value(table, table);
}

} // namespace

AdminWidget::AdminWidget(QWidget *parent)
    : QWidget(parent)
    , m_repo(new QueryRepository(DatabaseManager::instance().connectionName()))
{
    m_tabs = new QTabWidget(this);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_tabs);

    const QStringList tables = {
        QStringLiteral("workshop"),
        QStringLiteral("section"),
        QStringLiteral("brigade"),
        QStringLiteral("personnel_category"),
        QStringLiteral("product_category"),
        QStringLiteral("work_type"),
        QStringLiteral("laboratory"),
        QStringLiteral("equipment"),
        QStringLiteral("employee"),
        QStringLiteral("worker"),
        QStringLiteral("itp"),
        QStringLiteral("product_type"),
        QStringLiteral("product_instance"),
        QStringLiteral("production_cycle"),
        QStringLiteral("assembly_record"),
        QStringLiteral("test"),
        QStringLiteral("test_specialist"),
        QStringLiteral("test_equipment"),
        QStringLiteral("workshop_laboratory"),
        QStringLiteral("personnel_movement"),
    };

    for (const QString &table : tables) {
        m_tabs->addTab(new TableCrudWidget(table, {}, this), tableDisplayName(table));
    }

    m_tabs->addTab(buildProceduresTab(), tr("Процедуры"));
    m_tabs->addTab(buildReportsTab(), tr("Отчёты"));
}

QWidget *AdminWidget::buildProceduresTab()
{
    return new ProceduresHubWidget(m_repo, UserRole::Admin, this);
}

QWidget *AdminWidget::buildReportsTab()
{
    return new ReportsHubWidget(m_repo, UserRole::Admin, this);
}
