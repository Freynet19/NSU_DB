#include "db/QueryRepository.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
QueryRepository::QueryRepository(const QString &connectionName)
    : m_connectionName(connectionName)
{
}

QSqlDatabase QueryRepository::connection() const
{
    if (m_connectionName.isEmpty()) {
        return QSqlDatabase();
    }
    return QSqlDatabase::database(m_connectionName);
}

bool QueryRepository::execCall(const QString &sql, QString *errorMessage)
{
    QSqlQuery query(connection());
    if (!query.exec(sql)) {
        if (errorMessage) {
            *errorMessage = query.lastError().text();
        }
        return false;
    }
    return true;
}

QString QueryRepository::sqlLiteral(const QVariant &value) const
{
    if (value.isNull() || !value.isValid()) {
        return QStringLiteral("NULL");
    }
    if (value.typeId() == QMetaType::QString) {
        QString escaped = value.toString();
        escaped.replace(QLatin1Char('\''), QStringLiteral("''"));
        return QStringLiteral("'%1'").arg(escaped);
    }
    if (value.typeId() == QMetaType::QDate) {
        return QStringLiteral("DATE '%1'").arg(value.toDate().toString(Qt::ISODate));
    }
    if (value.canConvert<int>()) {
        return QString::number(value.toInt());
    }
    return QStringLiteral("'%1'").arg(value.toString());
}

QSqlQueryModel *QueryRepository::runSelectQuery(QSqlQuery &query, QString *errorMessage)
{
    if (!query.exec()) {
        if (errorMessage) {
            *errorMessage = query.lastError().text();
        }
        return nullptr;
    }

    auto *model = new QSqlQueryModel;
    model->setQuery(std::move(query));
    if (model->lastError().isValid()) {
        if (errorMessage) {
            *errorMessage = model->lastError().text();
        }
        delete model;
        return nullptr;
    }
    return model;
}

QSqlQueryModel *QueryRepository::executePrepared(const QString &statement,
                                                  const QVariantList &args,
                                                  QString *errorMessage)
{
    QStringList rendered;
    rendered.reserve(args.size());
    for (const QVariant &arg : args) {
        rendered.push_back(sqlLiteral(arg));
    }

    QString sql = QStringLiteral("EXECUTE %1").arg(statement);
    if (!args.isEmpty()) {
        sql += QLatin1Char('(') + rendered.join(QLatin1String(", ")) + QLatin1Char(')');
    }

    QSqlQuery query(connection());
    if (!query.exec(sql)) {
        if (errorMessage) {
            *errorMessage = query.lastError().text();
        }
        return nullptr;
    }

    auto *model = new QSqlQueryModel;
    model->setQuery(std::move(query));
    if (model->lastError().isValid()) {
        if (errorMessage) {
            *errorMessage = model->lastError().text();
        }
        delete model;
        return nullptr;
    }
    return model;
}

bool QueryRepository::callProcHireWorker(const QString &fullName,
                                         const QDate &birthDate,
                                         const QDate &hireDate,
                                         int categoryCode,
                                         const QString &specialty,
                                         int grade,
                                         std::optional<int> brigadeId,
                                         QString *errorMessage)
{
    const QString sql = QStringLiteral(
        "CALL proc_hire_worker(%1, %2, %3, %4, %5, %6, %7)")
                            .arg(sqlLiteral(fullName),
                                 sqlLiteral(birthDate),
                                 sqlLiteral(hireDate),
                                 sqlLiteral(categoryCode),
                                 sqlLiteral(specialty),
                                 sqlLiteral(grade),
                                 brigadeId ? sqlLiteral(*brigadeId) : QStringLiteral("NULL"));
    return execCall(sql, errorMessage);
}

bool QueryRepository::callProcHireItp(const QString &fullName,
                                      const QDate &birthDate,
                                      const QDate &hireDate,
                                      int categoryCode,
                                      const QString &position,
                                      const QString &qualification,
                                      QString *errorMessage)
{
    const QString sql = QStringLiteral(
        "CALL proc_hire_itp(%1, %2, %3, %4, %5, %6)")
                            .arg(sqlLiteral(fullName),
                                 sqlLiteral(birthDate),
                                 sqlLiteral(hireDate),
                                 sqlLiteral(categoryCode),
                                 sqlLiteral(position),
                                 qualification.isEmpty() ? QStringLiteral("NULL")
                                                         : sqlLiteral(qualification));
    return execCall(sql, errorMessage);
}

bool QueryRepository::callProcTransferEmployee(int employeeId,
                                               std::optional<int> newBrigadeId,
                                               std::optional<int> newSectionId,
                                               const QString &description,
                                               QString *errorMessage)
{
    const QString sql = QStringLiteral(
        "CALL proc_transfer_employee(%1, %2, %3, %4)")
                            .arg(sqlLiteral(employeeId),
                                 newBrigadeId ? sqlLiteral(*newBrigadeId) : QStringLiteral("NULL"),
                                 newSectionId ? sqlLiteral(*newSectionId) : QStringLiteral("NULL"),
                                 description.isEmpty() ? QStringLiteral("NULL")
                                                       : sqlLiteral(description));
    return execCall(sql, errorMessage);
}

bool QueryRepository::callProcDismissEmployee(int employeeId,
                                              const QString &description,
                                              QString *errorMessage)
{
    const QString sql = QStringLiteral("CALL proc_dismiss_employee(%1, %2)")
                            .arg(sqlLiteral(employeeId),
                                 description.isEmpty() ? QStringLiteral("NULL")
                                                       : sqlLiteral(description));
    return execCall(sql, errorMessage);
}

bool QueryRepository::callProcAssignBrigadeToStage(int instanceId,
                                                   int stageId,
                                                   int brigadeId,
                                                   const QDate &startDate,
                                                   QString *errorMessage)
{
    const QString sql = QStringLiteral(
        "CALL proc_assign_brigade_to_stage(%1, %2, %3, %4)")
                            .arg(sqlLiteral(instanceId),
                                 sqlLiteral(stageId),
                                 sqlLiteral(brigadeId),
                                 sqlLiteral(startDate));
    return execCall(sql, errorMessage);
}

bool QueryRepository::callProcCompleteStage(int recordId,
                                            const QDate &endDate,
                                            QString *errorMessage)
{
    const QString sql = QStringLiteral("CALL proc_complete_stage(%1, %2)")
                            .arg(sqlLiteral(recordId), sqlLiteral(endDate));
    return execCall(sql, errorMessage);
}

bool QueryRepository::callProcRegisterTest(int instanceId,
                                           int laboratoryId,
                                           const QDate &testDate,
                                           QString *errorMessage)
{
    const QString sql = QStringLiteral("CALL proc_register_test(%1, %2, %3)")
                            .arg(sqlLiteral(instanceId),
                                 sqlLiteral(laboratoryId),
                                 sqlLiteral(testDate));
    return execCall(sql, errorMessage);
}

bool QueryRepository::callProcCompleteTest(int testId,
                                           const QString &result,
                                           QString *errorMessage)
{
    const QString sql = QStringLiteral("CALL proc_complete_test(%1, %2)")
                            .arg(sqlLiteral(testId), sqlLiteral(result));
    return execCall(sql, errorMessage);
}

QSqlQueryModel *QueryRepository::query1ProductTypes(std::optional<int> workshopId,
                                                    std::optional<int> categoryId,
                                                    QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_product_types"),
                           {workshopId ? QVariant(*workshopId) : QVariant(),
                            categoryId ? QVariant(*categoryId) : QVariant()},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query2ProductCount(std::optional<int> workshopId,
                                                    std::optional<int> sectionId,
                                                    std::optional<int> categoryId,
                                                    const QDate &from,
                                                    const QDate &to,
                                                    QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_product_count"),
                           {workshopId ? QVariant(*workshopId) : QVariant(),
                            sectionId ? QVariant(*sectionId) : QVariant(),
                            categoryId ? QVariant(*categoryId) : QVariant(),
                            from,
                            to},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query2ProductList(std::optional<int> workshopId,
                                                  std::optional<int> sectionId,
                                                  std::optional<int> categoryId,
                                                  const QDate &from,
                                                  const QDate &to,
                                                  QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_product_list"),
                           {workshopId ? QVariant(*workshopId) : QVariant(),
                            sectionId ? QVariant(*sectionId) : QVariant(),
                            categoryId ? QVariant(*categoryId) : QVariant(),
                            from,
                            to},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query3Personnel(std::optional<int> workshopId,
                                                 std::optional<int> sectionId,
                                                 const QString &personnelType,
                                                 QString *errorMessage)
{
    QVariant typeArg;
    if (!personnelType.isEmpty()) {
        typeArg = personnelType;
    }
    return executePrepared(QStringLiteral("get_personnel_data"),
                           {workshopId ? QVariant(*workshopId) : QVariant(),
                            sectionId ? QVariant(*sectionId) : QVariant(),
                            typeArg},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query4SectionCount(std::optional<int> workshopId,
                                                    QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_section_count"),
                           {workshopId ? QVariant(*workshopId) : QVariant()},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query4SectionList(std::optional<int> workshopId,
                                                   QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_section_list"),
                           {workshopId ? QVariant(*workshopId) : QVariant()},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query5ProductWorks(int instanceId, QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_product_works"), {instanceId}, errorMessage);
}

QSqlQueryModel *QueryRepository::query6BrigadeComposition(std::optional<int> workshopId,
                                                          std::optional<int> sectionId,
                                                          QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_brigade_composition"),
                           {workshopId ? QVariant(*workshopId) : QVariant(),
                            sectionId ? QVariant(*sectionId) : QVariant()},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query7SectionMasters(std::optional<int> workshopId,
                                                      std::optional<int> sectionId,
                                                      QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_section_masters"),
                           {workshopId ? QVariant(*workshopId) : QVariant(),
                            sectionId ? QVariant(*sectionId) : QVariant()},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query8CurrentProducts(std::optional<int> workshopId,
                                                       std::optional<int> sectionId,
                                                       std::optional<int> categoryId,
                                                       QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_current_products"),
                           {workshopId ? QVariant(*workshopId) : QVariant(),
                            sectionId ? QVariant(*sectionId) : QVariant(),
                            categoryId ? QVariant(*categoryId) : QVariant()},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query9ProductBrigades(int instanceId, QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_product_brigade_composition"), {instanceId},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query10ProductLaboratories(int instanceId,
                                                            QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_product_laboratories"), {instanceId},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query11TestedProducts(std::optional<int> laboratoryId,
                                                     std::optional<int> categoryId,
                                                     const QDate &from,
                                                     const QDate &to,
                                                     QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_tested_products_by_laboratory"),
                           {laboratoryId ? QVariant(*laboratoryId) : QVariant(),
                            categoryId ? QVariant(*categoryId) : QVariant(),
                            from,
                            to},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query12TestSpecialists(std::optional<int> laboratoryId,
                                                        const QDate &from,
                                                        const QDate &to,
                                                        std::optional<int> instanceId,
                                                        std::optional<int> categoryId,
                                                        QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_test_specialists_by_laboratory"),
                           {laboratoryId ? QVariant(*laboratoryId) : QVariant(),
                            from,
                            to,
                            instanceId ? QVariant(*instanceId) : QVariant(),
                            categoryId ? QVariant(*categoryId) : QVariant()},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query13TestEquipment(std::optional<int> laboratoryId,
                                                      const QDate &from,
                                                      const QDate &to,
                                                      std::optional<int> instanceId,
                                                      std::optional<int> categoryId,
                                                      QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_test_equipment_by_laboratory"),
                           {laboratoryId ? QVariant(*laboratoryId) : QVariant(),
                            from,
                            to,
                            instanceId ? QVariant(*instanceId) : QVariant(),
                            categoryId ? QVariant(*categoryId) : QVariant()},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query14CurrentCount(std::optional<int> workshopId,
                                                     std::optional<int> sectionId,
                                                     std::optional<int> categoryId,
                                                     QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_current_products_count"),
                           {workshopId ? QVariant(*workshopId) : QVariant(),
                            sectionId ? QVariant(*sectionId) : QVariant(),
                            categoryId ? QVariant(*categoryId) : QVariant()},
                           errorMessage);
}

QSqlQueryModel *QueryRepository::query14CurrentList(std::optional<int> workshopId,
                                                    std::optional<int> sectionId,
                                                    std::optional<int> categoryId,
                                                    QString *errorMessage)
{
    return executePrepared(QStringLiteral("get_current_products_list"),
                           {workshopId ? QVariant(*workshopId) : QVariant(),
                            sectionId ? QVariant(*sectionId) : QVariant(),
                            categoryId ? QVariant(*categoryId) : QVariant()},
                           errorMessage);
}
