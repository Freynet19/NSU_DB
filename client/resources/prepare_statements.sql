-- Report PREPARE statements (single source of truth): read through vw_*.
-- Loaded at login by DatabaseManager; not part of docker init.
-- Role access: db/init/04_roles_users.sql, db/init/05_grants_extend.sql.
-- Manual EXECUTE examples: учебный отчёт .md only.

PREPARE get_product_types(INTEGER, INTEGER) AS
SELECT DISTINCT type_id,
                model_name,
                category_name,
                workshop_name
FROM vw_product_types
WHERE ($1 IS NULL OR workshop_id = $1)
  AND ($2 IS NULL OR category_id = $2)
ORDER BY workshop_name,
         category_name,
         model_name;

PREPARE get_product_count(INTEGER, INTEGER, INTEGER, DATE, DATE) AS
SELECT COUNT(DISTINCT instance_id) AS product_count
FROM vw_finished_products v
WHERE ($1 IS NULL OR v.workshop_id = $1)
  AND ($3 IS NULL OR v.category_id = $3)
  AND v.end_date BETWEEN $4 AND $5
  AND (
    $2 IS NULL
        OR EXISTS (SELECT 1
                   FROM assembly_record ar
                            JOIN production_cycle pcy USING (stage_id)
                   WHERE ar.instance_id = v.instance_id
                     AND pcy.section_id = $2)
    );

PREPARE get_product_list(INTEGER, INTEGER, INTEGER, DATE, DATE) AS
SELECT DISTINCT instance_id,
                model_name,
                category_name,
                workshop_name,
                start_date,
                end_date,
                status
FROM vw_finished_products v
WHERE ($1 IS NULL OR v.workshop_id = $1)
  AND ($3 IS NULL OR v.category_id = $3)
  AND v.end_date BETWEEN $4 AND $5
  AND (
    $2 IS NULL
        OR EXISTS (SELECT 1
                   FROM assembly_record ar
                            JOIN production_cycle pcy USING (stage_id)
                   WHERE ar.instance_id = v.instance_id
                     AND pcy.section_id = $2)
    )
ORDER BY workshop_name,
         category_name,
         end_date,
         instance_id;

PREPARE get_personnel_data(INTEGER, INTEGER, employee_category_type) AS
SELECT personnel_type,
       employee_id,
       full_name,
       birth_date,
       hire_date,
       category_code,
       category_name,
       position_name,
       qualification_or_grade,
       workshop_name,
       section_name,
       brigade_name
FROM vw_personnel_data
WHERE ($3 IS NULL OR personnel_type = $3::TEXT)
  AND ($2 IS NULL OR category_code = $2)
  AND ($1 IS NULL OR workshop_id = $1)
ORDER BY personnel_type,
         category_name,
         full_name;

PREPARE get_section_count(INTEGER) AS
SELECT COUNT(*) AS section_count
FROM vw_sections
WHERE ($1 IS NULL OR workshop_id = $1);

PREPARE get_section_list(INTEGER) AS
SELECT section_id,
       section_name,
       workshop_id,
       workshop_name,
       head_id,
       head_full_name,
       head_position,
       head_qualification
FROM vw_sections
WHERE ($1 IS NULL OR workshop_id = $1)
ORDER BY workshop_name,
         section_name;

PREPARE get_product_works(INTEGER) AS
SELECT instance_id,
       model_name,
       stage_number,
       work_type_id,
       work_name,
       work_description,
       section_id,
       section_name
FROM vw_product_works
WHERE instance_id = $1
ORDER BY stage_number,
         work_name;

PREPARE get_brigade_composition(INTEGER, INTEGER) AS
SELECT brigade_id,
       brigade_name,
       section_id,
       section_name,
       workshop_id,
       workshop_name,
       employee_id,
       full_name,
       specialty,
       grade,
       is_foreman
FROM vw_brigade_composition
WHERE ($1 IS NULL OR section_id = $1)
  AND ($2 IS NULL OR workshop_id = $2)
ORDER BY workshop_name,
         section_name,
         brigade_name,
         CASE
             WHEN is_foreman = 'Да' THEN 0
             ELSE 1
             END,
         full_name;

PREPARE get_section_masters(INTEGER, INTEGER) AS
SELECT employee_id,
       full_name,
       position,
       qualification,
       section_id,
       section_name,
       workshop_id,
       workshop_name
FROM vw_section_masters
WHERE ($1 IS NULL OR section_id = $1)
  AND ($2 IS NULL OR workshop_id = $2)
ORDER BY workshop_name,
         section_name,
         full_name;

PREPARE get_current_products(INTEGER, INTEGER, INTEGER) AS
SELECT DISTINCT instance_id,
                type_id,
                model_name,
                category_id,
                category_name,
                section_id,
                section_name,
                workshop_id,
                workshop_name,
                record_id,
                start_date,
                assembly_status,
                product_status
FROM vw_current_products
WHERE ($1 IS NULL OR section_id = $1)
  AND ($2 IS NULL OR workshop_id = $2)
  AND ($3 IS NULL OR category_id = $3)
ORDER BY workshop_name,
         section_name,
         category_name,
         model_name,
         instance_id;

PREPARE get_product_brigade_composition(INTEGER) AS
SELECT brigade_id,
       brigade_name,
       section_id,
       section_name,
       workshop_id,
       workshop_name,
       employee_id,
       full_name,
       specialty,
       grade,
       is_foreman
FROM vw_brigade_composition v
WHERE EXISTS (SELECT 1
              FROM assembly_record ar
              WHERE ar.instance_id = $1
                AND ar.brigade_id = v.brigade_id)
ORDER BY workshop_name,
         section_name,
         brigade_name,
         CASE
             WHEN is_foreman = 'Да' THEN 0
             ELSE 1
             END,
         full_name;

PREPARE get_product_laboratories(INTEGER) AS
SELECT DISTINCT laboratory_id,
                laboratory_name,
                location
FROM vw_product_laboratories
WHERE instance_id = $1
ORDER BY laboratory_name;

PREPARE get_tested_products_by_laboratory(INTEGER, INTEGER, DATE, DATE) AS
SELECT DISTINCT instance_id,
                type_id,
                model_name,
                category_id,
                category_name,
                test_id,
                test_date,
                laboratory_id,
                laboratory_name,
                status
FROM vw_tested_products
WHERE laboratory_id = $1
  AND ($2 IS NULL OR category_id = $2)
  AND test_date BETWEEN $3 AND $4
ORDER BY category_name,
         model_name,
         test_date,
         instance_id;

PREPARE get_test_specialists_by_laboratory(INTEGER, DATE, DATE, INTEGER, INTEGER) AS
SELECT DISTINCT employee_id,
                full_name,
                position,
                qualification,
                laboratory_id,
                laboratory_name,
                location
FROM vw_test_specialists
WHERE laboratory_id = $1
  AND test_date BETWEEN $2 AND $3
  AND ($4 IS NULL OR instance_id = $4)
  AND ($5 IS NULL OR category_id = $5)
ORDER BY full_name;

PREPARE get_test_equipment_by_laboratory(INTEGER, DATE, DATE, INTEGER, INTEGER) AS
SELECT DISTINCT equipment_id,
                equipment_name,
                equipment_type,
                laboratory_id,
                laboratory_name,
                location
FROM vw_test_equipment
WHERE laboratory_id = $1
  AND test_date BETWEEN $2 AND $3
  AND ($4 IS NULL OR instance_id = $4)
  AND ($5 IS NULL OR category_id = $5)
ORDER BY equipment_name,
         equipment_id;

PREPARE get_current_products_list(INTEGER, INTEGER, INTEGER) AS
SELECT DISTINCT instance_id,
                type_id,
                model_name,
                category_id,
                category_name,
                section_id,
                section_name,
                workshop_id,
                workshop_name
FROM vw_current_products
WHERE ($1 IS NULL OR section_id = $1)
  AND ($2 IS NULL OR workshop_id = $2)
  AND ($3 IS NULL OR category_id = $3)
ORDER BY workshop_name,
         section_name,
         category_name,
         model_name,
         instance_id;

PREPARE get_current_products_count(INTEGER, INTEGER, INTEGER) AS
SELECT COUNT(DISTINCT instance_id) AS product_count
FROM vw_current_products
WHERE ($1 IS NULL OR section_id = $1)
  AND ($2 IS NULL OR workshop_id = $2)
  AND ($3 IS NULL OR category_id = $3);
