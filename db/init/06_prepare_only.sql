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

PREPARE get_product_laboratories(INTEGER) AS
    SELECT DISTINCT l.laboratory_id,
                    l.name AS laboratory_name,
                    l.location
    FROM test t
             JOIN laboratory l USING (laboratory_id)
    WHERE t.instance_id = $1
    ORDER BY l.name;

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
