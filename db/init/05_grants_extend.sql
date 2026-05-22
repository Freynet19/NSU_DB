-- Extended grants for Qt client roles (supplements 04_roles_users.sql)

-- HR: personnel procedures and underlying tables
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

-- Production reporting: reports 1, 2, 8, 10, 11, 14 (prepare_statements.sql → vw_*)
GRANT
    SELECT
    ON
    vw_product_types,
    vw_finished_products,
    vw_current_products,
    vw_product_laboratories,
    vw_tested_products
    TO role_production_report;

-- Report 2: section filter via EXISTS on assembly_record / production_cycle
GRANT
    SELECT
    ON
    assembly_record,
    production_cycle
    TO role_production_report;
