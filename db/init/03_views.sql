-- перечень типов изделий с указанием их категории и цеха-изготовителя
CREATE OR REPLACE VIEW vw_product_types AS
SELECT DISTINCT pt.type_id,
pt.model_name,
pcat.category_id,
pcat.name AS category_name,
w.workshop_id,
w.name    AS workshop_name
FROM product_type pt
JOIN product_category pcat USING (category_id)
JOIN workshop w USING (workshop_id)
JOIN product_instance pi USING (type_id);
-- сведения об изделиях, завершивших сборку, находящихся на испытании или уже выпущенных
CREATE OR REPLACE VIEW vw_finished_products AS
SELECT DISTINCT pi.instance_id,
pt.type_id,
pt.model_name,
pcat.category_id,
pcat.name AS category_name,
w.workshop_id,
w.name    AS workshop_name,
pi.start_date,
pi.end_date,
pi.status
FROM product_instance pi
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id)
JOIN workshop w USING (workshop_id)
WHERE pi.status IN ('assembled', 'testing', 'released');
-- обобщённые сведения о персонале предприятия, включая ИТР и рабочих, с указанием категории, должности, квалификации и места работы
CREATE OR REPLACE VIEW vw_personnel_data AS
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
LEFT JOIN section s
ON s.section_id = i.master_section_id
LEFT JOIN workshop w_sec USING (workshop_id)
LEFT JOIN workshop w_head
ON w_head.head_id = i.employee_id
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
LEFT JOIN workshop w USING (workshop_id)) AS q;
-- перечень участков с указанием цеха и сведений о назначенном начальнике участка
CREATE OR REPLACE VIEW vw_sections AS
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
LEFT JOIN itp i
ON i.employee_id = s.section_head_id
LEFT JOIN employee e USING (employee_id);
-- перечень работ, выполняемых при изготовлении изделия, с указанием этапов производства и ответственных участков
CREATE OR REPLACE VIEW vw_product_works AS
SELECT pi.instance_id,
pt.type_id,
pt.model_name,
pcy.stage_number,
wt.work_type_id,
wt.name        AS work_name,
wt.description AS work_description,
s.section_id,
s.name         AS section_name
FROM product_instance pi
JOIN product_type pt USING (type_id)
JOIN production_cycle pcy
ON pcy.product_type_id = pt.type_id
JOIN work_type wt USING (work_type_id)
JOIN section s USING (section_id);
-- сведения о составе бригад, включая рабочих, их специальности, разряды и признак бригадира
CREATE OR REPLACE VIEW vw_brigade_composition AS
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
LEFT JOIN employee e USING (employee_id);
-- сведения о мастерах участков с указанием участка и цеха, за которыми они закреплены
CREATE OR REPLACE VIEW vw_section_masters AS
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
JOIN section s
ON s.section_id = i.master_section_id
JOIN workshop w USING (workshop_id);
-- перечень изделий, находящихся в процессе сборки в текущий момент, с указанием категории, участка, цеха и состояния сборки
CREATE OR REPLACE VIEW vw_current_products AS
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
AND ar.status = 'in_progress';
-- перечень лабораторий, в которых проводились испытания изделий
CREATE OR REPLACE VIEW vw_product_laboratories AS
SELECT DISTINCT t.instance_id,
l.laboratory_id,
l.name AS laboratory_name,
l.location
FROM test t
JOIN laboratory l USING (laboratory_id);
-- сведения об изделиях, проходивших испытания, с указанием лаборатории, даты испытания и категории изделия
CREATE OR REPLACE VIEW vw_tested_products AS
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
JOIN product_category pcat USING (category_id);
-- сведения о специалистах, участвовавших в проведении испытаний изделий в лабораториях
CREATE OR REPLACE VIEW vw_test_specialists AS
SELECT DISTINCT i.employee_id,
e.full_name,
i.position,
i.qualification,
t.test_id,
t.test_date,
t.instance_id,
l.laboratory_id,
l.name    AS laboratory_name,
l.location,
pcat.category_id,
pcat.name AS category_name
FROM test_specialist ts
JOIN test t USING (test_id)
JOIN laboratory l USING (laboratory_id)
JOIN itp i USING (employee_id)
JOIN employee e USING (employee_id)
JOIN product_instance pi USING (instance_id)
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id);
-- сведения об оборудовании, использовавшемся при проведении испытаний изделий в лабораториях
CREATE OR REPLACE VIEW vw_test_equipment AS
SELECT DISTINCT e.equipment_id,
e.name    AS equipment_name,
e.type    AS equipment_type,
t.test_id,
t.test_date,
t.instance_id,
l.laboratory_id,
l.name    AS laboratory_name,
l.location,
pcat.category_id,
pcat.name AS category_name
FROM test_equipment te
JOIN test t USING (test_id)
JOIN equipment e USING (equipment_id)
JOIN laboratory l
ON l.laboratory_id = e.laboratory_id
JOIN product_instance pi USING (instance_id)
JOIN product_type pt USING (type_id)
JOIN product_category pcat USING (category_id)
WHERE t.laboratory_id = e.laboratory_id;
