#pragma once

#include <QDate>
#include <QSqlQueryModel>
#include <QString>
#include <QVariant>
#include <optional>

class QueryRepository
{
public:
    explicit QueryRepository(const QString &connectionName);

    bool callProcHireWorker(const QString &fullName,
                            const QDate &birthDate,
                            const QDate &hireDate,
                            int categoryCode,
                            const QString &specialty,
                            int grade,
                            std::optional<int> brigadeId,
                            QString *errorMessage = nullptr);

    bool callProcHireItp(const QString &fullName,
                         const QDate &birthDate,
                         const QDate &hireDate,
                         int categoryCode,
                         const QString &position,
                         const QString &qualification,
                         QString *errorMessage = nullptr);

    bool callProcTransferEmployee(int employeeId,
                                  std::optional<int> newBrigadeId,
                                  std::optional<int> newSectionId,
                                  const QString &description,
                                  QString *errorMessage = nullptr);

    bool callProcDismissEmployee(int employeeId,
                                 const QString &description,
                                 QString *errorMessage = nullptr);

    bool callProcAssignBrigadeToStage(int instanceId,
                                      int stageId,
                                      int brigadeId,
                                      const QDate &startDate,
                                      QString *errorMessage = nullptr);

    bool callProcCompleteStage(int recordId,
                               const QDate &endDate,
                               QString *errorMessage = nullptr);

    bool callProcRegisterTest(int instanceId,
                              int laboratoryId,
                              const QDate &testDate,
                              QString *errorMessage = nullptr);

    bool callProcCompleteTest(int testId,
                              const QString &result,
                              QString *errorMessage = nullptr);

    QSqlQueryModel *query1ProductTypes(std::optional<int> workshopId,
                                       std::optional<int> categoryId,
                                       QString *errorMessage = nullptr);

    QSqlQueryModel *query2ProductCount(std::optional<int> workshopId,
                                       std::optional<int> sectionId,
                                       std::optional<int> categoryId,
                                       const QDate &from,
                                       const QDate &to,
                                       QString *errorMessage = nullptr);

    QSqlQueryModel *query2ProductList(std::optional<int> workshopId,
                                      std::optional<int> sectionId,
                                      std::optional<int> categoryId,
                                      const QDate &from,
                                      const QDate &to,
                                      QString *errorMessage = nullptr);

    QSqlQueryModel *query3Personnel(std::optional<int> workshopId,
                                    std::optional<int> categoryCode,
                                    const QString &personnelType,
                                    QString *errorMessage = nullptr);

    QSqlQueryModel *query4SectionCount(std::optional<int> workshopId,
                                       QString *errorMessage = nullptr);

    QSqlQueryModel *query4SectionList(std::optional<int> workshopId,
                                      QString *errorMessage = nullptr);

    QSqlQueryModel *query5ProductWorks(int instanceId, QString *errorMessage = nullptr);

    QSqlQueryModel *query6BrigadeComposition(std::optional<int> workshopId,
                                             std::optional<int> sectionId,
                                             QString *errorMessage = nullptr);

    QSqlQueryModel *query7SectionMasters(std::optional<int> workshopId,
                                         std::optional<int> sectionId,
                                         QString *errorMessage = nullptr);

    QSqlQueryModel *query8CurrentProducts(std::optional<int> workshopId,
                                          std::optional<int> sectionId,
                                          std::optional<int> categoryId,
                                          QString *errorMessage = nullptr);

    QSqlQueryModel *query9ProductBrigades(int instanceId, QString *errorMessage = nullptr);

    QSqlQueryModel *query10ProductLaboratories(int instanceId, QString *errorMessage = nullptr);

    QSqlQueryModel *query11TestedProducts(std::optional<int> laboratoryId,
                                          std::optional<int> categoryId,
                                          const QDate &from,
                                          const QDate &to,
                                          QString *errorMessage = nullptr);

    QSqlQueryModel *query12TestSpecialists(std::optional<int> laboratoryId,
                                           const QDate &from,
                                           const QDate &to,
                                           std::optional<int> instanceId,
                                           std::optional<int> categoryId,
                                           QString *errorMessage = nullptr);

    QSqlQueryModel *query13TestEquipment(std::optional<int> laboratoryId,
                                         const QDate &from,
                                         const QDate &to,
                                         std::optional<int> instanceId,
                                         std::optional<int> categoryId,
                                         QString *errorMessage = nullptr);

    QSqlQueryModel *query14CurrentCount(std::optional<int> workshopId,
                                        std::optional<int> sectionId,
                                        std::optional<int> categoryId,
                                        QString *errorMessage = nullptr);

    QSqlQueryModel *query14CurrentList(std::optional<int> workshopId,
                                       std::optional<int> sectionId,
                                       std::optional<int> categoryId,
                                       QString *errorMessage = nullptr);

private:
    bool execCall(const QString &sql, QString *errorMessage);
    QSqlQueryModel *executePrepared(const QString &statement,
                                    const QVariantList &args,
                                    QString *errorMessage);
    QString sqlLiteral(const QVariant &value) const;
    QSqlDatabase connection() const;

    QString m_connectionName;
};
