cmake_minimum_required(VERSION 3.21)

project(automotive_erp_client VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

qt_add_executable(automotive_erp_client
    src/main.cpp
    src/core/UserRole.cpp
    src/db/DatabaseManager.cpp
    src/db/QueryRepository.cpp
    src/db/LookupRepository.cpp
    src/crud/CrudTableCatalog.cpp
    src/reports/ReportCatalog.cpp
    src/reports/ReportFilterBuilder.cpp
    src/procedures/ProcedureCatalog.cpp
    src/procedures/ProcedureFormBuilder.cpp
    src/auth/LoginDialog.cpp
    src/shell/MainWindow.cpp
    src/widgets/TableCrudWidget.cpp
    src/widgets/CrudHubWidget.cpp
    src/widgets/ReportTableWidget.cpp
    src/widgets/ReportsHubWidget.cpp
    src/widgets/ProcedureFormWidget.cpp
    src/widgets/ProceduresHubWidget.cpp
    src/roles/AdminWidget.cpp
    src/roles/HrWidget.cpp
    src/roles/ProductionReportWidget.cpp
    ui/LoginDialog.ui
    ui/MainWindow.ui
    ui/ReportTableWidget.ui
    ui/ReportsHubWidget.ui
    ui/ProceduresHubWidget.ui
    ui/TableCrudWidget.ui
    ui/CrudHubWidget.ui
    resources/app.qrc
)

target_include_directories(automotive_erp_client PRIVATE
    src
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(automotive_erp_client PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)

target_link_libraries(automotive_erp_client PRIVATE Qt6::Widgets Qt6::Sql)

if(NOT TARGET Qt6::QPSQLDriverPlugin AND NOT Qt6_QPSQL_FOUND)
    message(STATUS "Ensure libqt6-sql6-psql is installed for PostgreSQL connectivity")
endif()
