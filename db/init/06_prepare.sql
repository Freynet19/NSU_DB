PREPARE get_product_types(INTEGER, INTEGER) AS
SELECT DISTINCT pt.type_id,
pt.model_name,
pcat.name AS category_name,
w.name    AS workshop_name
FROM product_type pt
JOIN product_category pcat USING (category_id)
JOIN workshop w USING (workshop_id)
JOIN product_instance pi USING (type_id)
WHERE ($1 IS NULL OR pt.workshop_id = $1)
AND ($2 IS NULL OR pt.category_id = $2)
ORDER BY w.name,
pcat.name,
pt.model_name;
-- 1. виды изделий заданной категории указанного цеха
EXECUTE get_product_types(1, 1);
-- 2. все виды изделий указанного цеха
EXECUTE get_product_types(1, NULL);
-- 3. виды изделий заданной категории по предприятию в целом
EXECUTE get_product_types(NULL, 1);
-- 4. все виды изделий по предприятию в целом
EXECUTE get_product_types(NULL, NULL);
PREPARE get_product_count(INTEGER, INTEGER, INTEGER, DATE, DATE) AS
SELECT COUNT(DISTINCT pi.instance_id) AS product_count
FROM product_instance pi
JOIN product_type pt USING (type_id)
WHERE ($1 IS NULL OR pt.workshop_id = $1)
AND ($3 IS NULL OR pt.category_id = $3)
AND pi.end_date BETWEEN $4 AND $5
AND pi.status IN ('assembled', 'testing', 'released')
AND (
$2 IS NULL
OR EXISTS (SELECT 1
FROM assembly_record ar
JOIN production_cycle pcy USING (stage_id)
WHERE ar.instance_id = pi.instance_id
AND pcy.section_id = $2)
);
PREPARE get_product_list(INTEGER, INTEGER, INTEGER, DATE, DATE) AS
SELECT DISTINCT pi.instance_id,
pt.model_name,
pcat.name AS category_name,
w.name    AS workshop_name,
pi.start_date,
pi.end_date,
pi.status
FROM product_instance pi
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id)
JOIN workshop w USING (workshop_id)
WHERE ($1 IS NULL OR pt.workshop_id = $1)
AND ($3 IS NULL OR pt.category_id = $3)
AND pi.end_date BETWEEN $4 AND $5
AND pi.status IN ('assembled', 'testing', 'released')
AND (
$2 IS NULL
OR EXISTS (SELECT 1
FROM assembly_record ar
JOIN production_cycle pcy USING (stage_id)
WHERE ar.instance_id = pi.instance_id
AND pcy.section_id = $2)
)
ORDER BY w.name,
pcat.name,
pi.end_date,
pi.instance_id;
-- 1. заданная категория указанного цеха
EXECUTE get_product_count(1, NULL, 1, DATE '2026-01-01', DATE '2026-12-31');
EXECUTE get_product_list (1, NULL, 1, DATE '2026-01-01', DATE '2026-12-31');
-- 2. все изделия указанного цеха
EXECUTE get_product_count(1, NULL, NULL, DATE '2026-01-01', DATE '2026-12-31');
EXECUTE get_product_list (1, NULL, NULL, DATE '2026-01-01', DATE '2026-12-31');
-- 3. заданная категория указанного участка
EXECUTE get_product_count(NULL, 4, 3, DATE '2026-01-01', DATE '2026-12-31');
EXECUTE get_product_list (NULL, 4, 3, DATE '2026-01-01', DATE '2026-12-31');
-- 4. все изделия указанного участка
EXECUTE get_product_count(NULL, 4, NULL, DATE '2026-01-01', DATE '2026-12-31');
EXECUTE get_product_list (NULL, 4, NULL, DATE '2026-01-01', DATE '2026-12-31');
-- 5. заданная категория по предприятию в целом
EXECUTE get_product_count(NULL, NULL, 1, DATE '2026-01-01', DATE '2026-12-31');
EXECUTE get_product_list (NULL, NULL, 1, DATE '2026-01-01', DATE '2026-12-31');
-- 6. все изделия по предприятию в целом
EXECUTE get_product_count(NULL, NULL, NULL, DATE '2026-01-01', DATE '2026-12-31');
EXECUTE get_product_list (NULL, NULL, NULL, DATE '2026-01-01', DATE '2026-12-31');
PREPARE get_personnel_data(INTEGER, INTEGER, employee_category_type) AS
SELECT q.personnel_type,
q.employee_id,
q.full_name,
q.birth_date,
q.hire_date,
q.category_code,
q.category_name,
q.position_name,
q.qualification_or_grade,
q.workshop_name,
q.section_name,
q.brigade_name
FROM (SELECT DISTINCT 'ITP'                             AS personnel_type,
e.employee_id,
e.full_name,
e.birth_date,
e.hire_date,
pcat.category_code,
pcat.name                         AS category_name,
i.position                        AS position_name,
i.qualification                   AS qualification_or_grade,
COALESCE(w_head.name, w_sec.name) AS workshop_name,
s.name                            AS section_name,
NULL::VARCHAR(100)                AS brigade_name
FROM employee e
JOIN itp i USING (employee_id)
JOIN personnel_category pcat USING (category_code)
LEFT JOIN section s ON s.section_id = i.master_section_id
LEFT JOIN workshop w_sec USING (workshop_id)
LEFT JOIN workshop w_head ON w_head.head_id = i.employee_id
WHERE ($3 IS NULL OR $3 = 'ITP')
AND ($2 IS NULL OR e.category_code = $2)
AND (
$1 IS NULL
OR w_sec.workshop_id = $1
OR w_head.workshop_id = $1
)
UNION ALL
SELECT 'Worker'              AS personnel_type,
e.employee_id,
e.full_name,
e.birth_date,
e.hire_date,
pcat.category_code,
pcat.name             AS category_name,
wr.specialty          AS position_name,
wr.grade::VARCHAR(20) AS qualification_or_grade,
w.name                AS workshop_name,
s.name                AS section_name,
b.name                AS brigade_name
FROM employee e
JOIN worker wr USING (employee_id)
JOIN personnel_category pcat USING (category_code)
LEFT JOIN brigade b USING (brigade_id)
LEFT JOIN section s USING (section_id)
LEFT JOIN workshop w USING (workshop_id)
WHERE ($3 IS NULL OR $3 = 'Worker')
AND ($2 IS NULL OR e.category_code = $2)
AND ($1 IS NULL OR w.workshop_id = $1)) AS q
ORDER BY q.personnel_type,
q.category_name,
q.full_name;
-- 1. весь кадровый состав указанного цеха
EXECUTE get_personnel_data(1, NULL, NULL);
-- 2. инженерно-технический персонал указанного цеха
EXECUTE get_personnel_data(1, NULL, 'ITP');
-- 3. рабочие указанного цеха
EXECUTE get_personnel_data(1, NULL, 'Worker');
-- 4. инженерно-технический персонал указанного цеха по заданной категории
EXECUTE get_personnel_data(1, 8, 'ITP');
-- 5. рабочие указанного цеха по заданной категории
EXECUTE get_personnel_data(1, 1, 'Worker');
-- 6. весь кадровый состав предприятия
EXECUTE get_personnel_data(NULL, NULL, NULL);
-- 7. инженерно-технический персонал предприятия
EXECUTE get_personnel_data(NULL, NULL, 'ITP');
-- 8. рабочие предприятия
EXECUTE get_personnel_data(NULL, NULL, 'Worker');
-- 9. инженерно-технический персонал предприятия по заданной категории
EXECUTE get_personnel_data(NULL, 8, 'ITP');
-- 10. рабочие предприятия по заданной категории
EXECUTE get_personnel_data(NULL, 1, 'Worker');
PREPARE get_section_count(INTEGER) AS
SELECT COUNT(*) AS section_count
FROM section s
WHERE ($1 IS NULL OR s.workshop_id = $1);
PREPARE get_section_list(INTEGER) AS
SELECT s.section_id,
s.name          AS section_name,
w.workshop_id,
w.name          AS workshop_name,
i.employee_id   AS head_id,
e.full_name     AS head_full_name,
i.position      AS head_position,
i.qualification AS head_qualification
FROM section s
JOIN workshop w USING (workshop_id)
LEFT JOIN itp i ON i.employee_id = s.section_head_id
LEFT JOIN employee e USING (employee_id)
WHERE ($1 IS NULL OR s.workshop_id = $1)
ORDER BY w.name,
s.name;
-- 1. Число и перечень участков указанного цеха
EXECUTE get_section_count(1);
EXECUTE get_section_list(1);
-- 2. Число и перечень участков предприятия в целом
EXECUTE get_section_count(NULL);
EXECUTE get_section_list(NULL);
PREPARE get_product_works(INTEGER) AS
SELECT pi.instance_id,
pt.model_name,
pcy.stage_number,
wt.work_type_id,
wt.name        AS work_name,
wt.description AS work_description,
s.section_id,
s.name         AS section_name
FROM product_instance pi
JOIN product_type pt USING (type_id)
JOIN production_cycle pcy ON pcy.product_type_id = pt.type_id
JOIN work_type wt USING (work_type_id)
JOIN section s USING (section_id)
WHERE pi.instance_id = $1
ORDER BY pcy.stage_number,
wt.name;
EXECUTE get_product_works(1);
PREPARE get_brigade_composition(INTEGER, INTEGER) AS
SELECT b.brigade_id,
b.name  AS brigade_name,
s.section_id,
s.name  AS section_name,
w.workshop_id,
w.name  AS workshop_name,
wr.employee_id,
e.full_name,
wr.specialty,
wr.grade,
CASE
WHEN b.foreman_id = wr.employee_id THEN 'Да'
ELSE 'Нет'
END AS is_foreman
FROM brigade b
JOIN section s USING (section_id)
JOIN workshop w USING (workshop_id)
LEFT JOIN worker wr USING (brigade_id)
LEFT JOIN employee e USING (employee_id)
WHERE ($1 IS NULL OR s.section_id = $1)
AND ($2 IS NULL OR s.workshop_id = $2)
ORDER BY w.name,
s.name,
b.name,
CASE
WHEN b.foreman_id = wr.employee_id THEN 0
ELSE 1
END,
e.full_name;
-- 1. состав бригад указанного участка
EXECUTE get_brigade_composition(4, NULL);
-- 2. состав бригад указанного цеха
EXECUTE get_brigade_composition(NULL, 1);
-- 3. состав бригад указанного участка указанного цеха
EXECUTE get_brigade_composition(4, 1);
PREPARE get_section_masters(INTEGER, INTEGER) AS
SELECT i.employee_id,
e.full_name,
i.position,
i.qualification,
s.section_id,
s.name AS section_name,
w.workshop_id,
w.name AS workshop_name
FROM itp i
JOIN employee e USING (employee_id)
JOIN section s ON s.section_id = i.master_section_id
JOIN workshop w USING (workshop_id)
WHERE ($1 IS NULL OR s.section_id = $1)
AND ($2 IS NULL OR s.workshop_id = $2)
ORDER BY w.name,
s.name,
e.full_name;
-- 1. перечень мастеров указанного участка
EXECUTE get_section_masters(4, NULL);
-- 2. перечень мастеров указанного цеха
EXECUTE get_section_masters(NULL, 1);
-- 3. перечень мастеров указанного участка указанного цеха
EXECUTE get_section_masters(4, 1);
PREPARE get_current_products(INTEGER, INTEGER, INTEGER) AS
SELECT DISTINCT pi.instance_id,
pt.type_id,
pt.model_name,
pcat.category_id,
pcat.name AS category_name,
s.section_id,
s.name    AS section_name,
w.workshop_id,
w.name    AS workshop_name,
ar.record_id,
ar.start_date,
ar.status AS assembly_status,
pi.status AS product_status
FROM product_instance pi
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id)
JOIN assembly_record ar USING (instance_id)
JOIN brigade b USING (brigade_id)
JOIN section s USING (section_id)
JOIN workshop w ON w.workshop_id = s.workshop_id
WHERE pi.status = 'assembling'
AND ar.status = 'in_progress'
AND ($1 IS NULL OR s.section_id = $1)
AND ($2 IS NULL OR w.workshop_id = $2)
AND ($3 IS NULL OR pt.category_id = $3)
ORDER BY w.name,
s.name,
pcat.name,
pt.model_name,
pi.instance_id;
-- 1. изделия заданной категории, собираемые в настоящий момент указанным участком
EXECUTE get_current_products(4, NULL, 1);
-- 2. все изделия, собираемые в настоящий момент указанным участком
EXECUTE get_current_products(4, NULL, NULL);
-- 3. изделия заданной категории, собираемые в настоящий момент указанным цехом
EXECUTE get_current_products(NULL, 1, 1);
-- 4. все изделия, собираемые в настоящий момент указанным цехом
EXECUTE get_current_products(NULL, 1, NULL);
-- 5. изделия заданной категории, собираемые в настоящий момент предприятием в целом
EXECUTE get_current_products(NULL, NULL, 1);
-- 6. все изделия, собираемые в настоящий момент предприятием в целом
EXECUTE get_current_products(NULL, NULL, NULL);
PREPARE get_product_brigade_composition(INTEGER) AS
SELECT b.brigade_id,
b.name  AS brigade_name,
s.section_id,
s.name  AS section_name,
w.workshop_id,
w.name  AS workshop_name,
wr.employee_id,
e.full_name,
wr.specialty,
wr.grade,
CASE
WHEN b.foreman_id = wr.employee_id THEN 'Да'
ELSE 'Нет'
END AS is_foreman
FROM brigade b
JOIN section s USING (section_id)
JOIN workshop w USING (workshop_id)
LEFT JOIN worker wr USING (brigade_id)
LEFT JOIN employee e USING (employee_id)
WHERE EXISTS (SELECT 1
FROM assembly_record ar
WHERE ar.instance_id = $1
AND ar.brigade_id = b.brigade_id)
ORDER BY w.name,
s.name,
b.name,
CASE
WHEN b.foreman_id = wr.employee_id THEN 0
ELSE 1
END,
e.full_name;
EXECUTE get_product_brigade_composition(1);
PREPARE get_product_laboratories(INTEGER) AS
SELECT DISTINCT l.laboratory_id,
l.name AS laboratory_name,
l.location
FROM test t
JOIN laboratory l USING (laboratory_id)
WHERE t.instance_id = $1
ORDER BY l.name;
EXECUTE get_product_laboratories(1);
PREPARE get_tested_products_by_laboratory(INTEGER, INTEGER, DATE, DATE) AS
SELECT DISTINCT pi.instance_id,
pt.type_id,
pt.model_name,
pcat.category_id,
pcat.name AS category_name,
t.test_id,
t.test_date,
l.laboratory_id,
l.name    AS laboratory_name,
pi.status
FROM test t
JOIN laboratory l USING (laboratory_id)
JOIN product_instance pi USING (instance_id)
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id)
WHERE t.laboratory_id = $1
AND ($2 IS NULL OR pt.category_id = $2)
AND t.test_date BETWEEN $3 AND $4
ORDER BY pcat.name,
pt.model_name,
t.test_date,
pi.instance_id;
-- 1. изделия заданной категории, проходившие испытание в указанной лаборатории за период
EXECUTE get_tested_products_by_laboratory(1, 1, DATE '2026-01-01', DATE '2026-12-31');
-- 2. все изделия, проходившие испытание в указанной лаборатории за период
EXECUTE get_tested_products_by_laboratory(1, NULL, DATE '2026-01-01', DATE '2026-12-31');
PREPARE get_test_specialists_by_laboratory(INTEGER, DATE, DATE, INTEGER, INTEGER) AS
SELECT DISTINCT i.employee_id,
e.full_name,
i.position,
i.qualification,
l.laboratory_id,
l.name AS laboratory_name,
l.location
FROM test_specialist ts
JOIN test t USING (test_id)
JOIN laboratory l USING (laboratory_id)
JOIN itp i USING (employee_id)
JOIN employee e USING (employee_id)
JOIN product_instance pi USING (instance_id)
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id)
WHERE t.laboratory_id = $1
AND t.test_date BETWEEN $2 AND $3
AND ($4 IS NULL OR t.instance_id = $4)
AND ($5 IS NULL OR pt.category_id = $5)
ORDER BY e.full_name;
-- 1. Испытатели, участвующие в испытаниях указанного изделия
EXECUTE get_test_specialists_by_laboratory(1, DATE '2026-01-01', DATE '2026-12-31', 5, NULL);
-- 2. Испытатели, участвующие в испытаниях изделий заданной категории
EXECUTE get_test_specialists_by_laboratory(1, DATE '2026-01-01', DATE '2026-12-31', NULL, 3);
-- 3. Испытатели, участвующие в испытаниях всех изделий
EXECUTE get_test_specialists_by_laboratory(1, DATE '2026-01-01', DATE '2026-12-31', NULL, NULL);
PREPARE get_test_equipment_by_laboratory(INTEGER, DATE, DATE, INTEGER, INTEGER) AS
SELECT DISTINCT e.equipment_id,
e.name AS equipment_name,
e.type AS equipment_type,
l.laboratory_id,
l.name AS laboratory_name,
l.location
FROM test_equipment te
JOIN test t USING (test_id)
JOIN equipment e USING (equipment_id)
JOIN laboratory l ON l.laboratory_id = e.laboratory_id
JOIN product_instance pi USING (instance_id)
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id)
WHERE t.laboratory_id = $1
AND e.laboratory_id = $1
AND t.test_date BETWEEN $2 AND $3
AND ($4 IS NULL OR t.instance_id = $4)
AND ($5 IS NULL OR pt.category_id = $5)
ORDER BY e.name,
e.equipment_id;
-- 1. оборудование, использовавшееся при испытании указанного изделия
EXECUTE get_test_equipment_by_laboratory(1, DATE '2026-01-01', DATE '2026-12-31', 5, NULL);
-- 2. оборудование, использовавшееся при испытании изделий отдельной категории
EXECUTE get_test_equipment_by_laboratory(1, DATE '2026-01-01', DATE '2026-12-31', NULL, 3);
-- 3. оборудование, использовавшееся при испытании всех изделий
EXECUTE get_test_equipment_by_laboratory(1, DATE '2026-01-01', DATE '2026-12-31', NULL, NULL);
PREPARE get_current_products_list(INTEGER, INTEGER, INTEGER) AS
SELECT DISTINCT pi.instance_id,
pt.type_id,
pt.model_name,
pcat.category_id,
pcat.name AS category_name,
s.section_id,
s.name    AS section_name,
w.workshop_id,
w.name    AS workshop_name
FROM product_instance pi
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id)
JOIN assembly_record ar USING (instance_id)
JOIN brigade b USING (brigade_id)
JOIN section s USING (section_id)
JOIN workshop w ON w.workshop_id = s.workshop_id
WHERE pi.status = 'assembling'
AND ar.status = 'in_progress'
AND ($1 IS NULL OR s.section_id = $1)
AND ($2 IS NULL OR w.workshop_id = $2)
AND ($3 IS NULL OR pcat.category_id = $3)
ORDER BY w.name,
s.name,
pcat.name,
pt.model_name,
pi.instance_id;
PREPARE get_current_products_count(INTEGER, INTEGER, INTEGER) AS
SELECT COUNT(DISTINCT pi.instance_id) AS product_count
FROM product_instance pi
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id)
JOIN assembly_record ar USING (instance_id)
JOIN brigade b USING (brigade_id)
JOIN section s USING (section_id)
JOIN workshop w ON w.workshop_id = s.workshop_id
WHERE pi.status = 'assembling'
AND ar.status = 'in_progress'
AND ($1 IS NULL OR s.section_id = $1)
AND ($2 IS NULL OR w.workshop_id = $2)
AND ($3 IS NULL OR pcat.category_id = $3);
-- 1. перечень и число изделий заданной категории, собираемых указанным участком
EXECUTE get_current_products_list(4, NULL, 2);
EXECUTE get_current_products_count(4, NULL, 2);
-- 2. перечень и число всех изделий, собираемых указанным участком
EXECUTE get_current_products_list(4, NULL, NULL);
EXECUTE get_current_products_count(4, NULL, NULL);
-- 3. перечень и число изделий заданной категории, собираемых указанным цехом
EXECUTE get_current_products_list(NULL, 1, 2);
EXECUTE get_current_products_count(NULL, 1, 2);
-- 4. перечень и число всех изделий, собираемых указанным цехом
EXECUTE get_current_products_list(NULL, 1, NULL);
EXECUTE get_current_products_count(NULL, 1, NULL);
-- 5. перечень и число изделий заданной категории, собираемых предприятием в целом
EXECUTE get_current_products_list(NULL, NULL, 2);
EXECUTE get_current_products_count(NULL, NULL, 2);
-- 6. перечень и число всех изделий, собираемых предприятием в целом
EXECUTE get_current_products_list(NULL, NULL, NULL);
EXECUTE get_current_products_count(NULL, NULL, NULL);
