-- Extended grants for Qt client roles (supplements 04_roles_users.sql)

-- HR: invoke personnel procedures and access underlying tables
GRANT
EXECUTE
ON
PROCEDURE
proc_hire_worker
(VARCHAR, DATE, DATE, INTEGER, VARCHAR, INTEGER, INTEGER),
    proc_hire_itp(VARCHAR, DATE, DATE, INTEGER, VARCHAR, VARCHAR),
    proc_transfer_employee(INTEGER, INTEGER, INTEGER, TEXT),
    proc_dismiss_employee(INTEGER, TEXT)
    TO role_hr;

GRANT INSERT,
UPDATE,
SELECT
ON
    employee, worker, itp, personnel_movement, brigade
    TO role_hr;

GRANT
USAGE,
SELECT
ON ALL SEQUENCES IN SCHEMA public TO role_hr;

-- Production reporting: read data for PREPARE queries and views
GRANT
SELECT
ON ALL TABLES IN SCHEMA public TO role_production_report;
GRANT
SELECT
ON ALL SEQUENCES IN SCHEMA public TO role_production_report;

GRANT
SELECT
ON
    vw_product_laboratories,
    vw_tested_products
    TO role_production_report;
