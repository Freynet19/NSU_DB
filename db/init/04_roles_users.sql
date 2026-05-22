-- групповые роли
CREATE ROLE role_admin NOLOGIN;
CREATE ROLE role_production_report NOLOGIN;
CREATE ROLE role_hr NOLOGIN;
CREATE ROLE role_workshop NOLOGIN;
CREATE ROLE role_laboratory NOLOGIN;
ALTER
    ROLE role_admin SET lock_timeout = '5s';
ALTER
    ROLE role_admin SET statement_timeout = '30s';
-- Пользователи workshop/laboratory — модель из отчёта; Qt-клиент их не использует (см. LoginDialog).

-- пользователи
CREATE
    USER user_admin WITH PASSWORD 'admin_password';
CREATE
    USER user_production WITH PASSWORD 'production_password';
CREATE
    USER user_hr WITH PASSWORD 'hr_password';
CREATE
    USER user_workshop WITH PASSWORD 'workshop_password';
CREATE
    USER user_laboratory WITH PASSWORD 'laboratory_password';
-- назначение ролей пользователям
GRANT role_admin TO user_admin;
GRANT role_production_report TO user_production;
GRANT role_hr TO user_hr;
GRANT role_workshop TO user_workshop;
GRANT role_laboratory TO user_laboratory;
-- базовые права на подключение и схему
GRANT
    CONNECT
    ON DATABASE test TO role_admin;
GRANT CONNECT
    ON DATABASE test TO role_production_report;
GRANT CONNECT
    ON DATABASE test TO role_hr;
GRANT CONNECT
    ON DATABASE test TO role_workshop;
GRANT CONNECT
    ON DATABASE test TO role_laboratory;
GRANT USAGE ON SCHEMA
    public TO role_admin;
GRANT USAGE ON SCHEMA
    public TO role_production_report;
GRANT USAGE ON SCHEMA
    public TO role_hr;
GRANT USAGE ON SCHEMA
    public TO role_workshop;
GRANT USAGE ON SCHEMA
    public TO role_laboratory;
-- полные права администратора
GRANT ALL PRIVILEGES ON ALL
    TABLES IN SCHEMA public TO role_admin;
GRANT ALL PRIVILEGES ON ALL
    SEQUENCES IN SCHEMA public TO role_admin;
GRANT ALL PRIVILEGES ON ALL
    FUNCTIONS IN SCHEMA public TO role_admin;
GRANT ALL PRIVILEGES ON ALL
    PROCEDURES IN SCHEMA public TO role_admin;
ALTER
    DEFAULT PRIVILEGES IN SCHEMA public
    GRANT ALL PRIVILEGES ON TABLES TO role_admin;
ALTER
    DEFAULT PRIVILEGES IN SCHEMA public
    GRANT ALL PRIVILEGES ON SEQUENCES TO role_admin;
ALTER
    DEFAULT PRIVILEGES IN SCHEMA public
    GRANT ALL PRIVILEGES ON FUNCTIONS TO role_admin;
ALTER
    DEFAULT PRIVILEGES IN SCHEMA public
    GRANT ALL PRIVILEGES ON ROUTINES TO role_admin;
-- Права production_report для клиента — в 05_grants_extend.sql

-- права кадровой роли
GRANT
    SELECT
    ON
    vw_personnel_data,
    vw_sections,
    vw_brigade_composition,
    vw_section_masters
    TO role_hr;
-- права роли цеха
GRANT
    SELECT
    ON
    vw_product_types,
    vw_product_works,
    vw_brigade_composition,
    vw_current_products
    TO role_workshop;
-- права лабораторной роли
GRANT
    SELECT
    ON
    vw_product_laboratories,
    vw_tested_products,
    vw_test_specialists,
    vw_test_equipment
    TO role_laboratory;
