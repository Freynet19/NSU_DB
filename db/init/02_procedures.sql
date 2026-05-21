CREATE
OR REPLACE PROCEDURE proc_hire_worker(
p_full_name VARCHAR(200),
p_birth_date DATE,
p_hire_date DATE,
p_category_code INTEGER,
p_specialty VARCHAR(100),
p_grade INTEGER,
p_brigade_id INTEGER DEFAULT NULL
)
LANGUAGE plpgsql AS
$$
DECLARE
v_employee_id INTEGER;
BEGIN
INSERT INTO employee (full_name, birth_date, hire_date, category_code)
VALUES (p_full_name, p_birth_date, p_hire_date, p_category_code) RETURNING employee_id
INTO v_employee_id;
INSERT INTO worker (employee_id, specialty, grade, brigade_id)
VALUES (v_employee_id, p_specialty, p_grade, p_brigade_id);
INSERT INTO personnel_movement (employee_id, movement_type, date)
VALUES (v_employee_id, 'hire', p_hire_date);
END;
$$;
CREATE
OR REPLACE PROCEDURE proc_hire_itp(
p_full_name VARCHAR(200),
p_birth_date DATE,
p_hire_date DATE,
p_category_code INTEGER,
p_position VARCHAR(100),
p_qualification VARCHAR(100) DEFAULT NULL
)
LANGUAGE plpgsql AS
$$
DECLARE
v_employee_id INTEGER;
BEGIN
INSERT INTO employee (full_name, birth_date, hire_date, category_code)
VALUES (p_full_name, p_birth_date, p_hire_date, p_category_code) RETURNING employee_id
INTO v_employee_id;
INSERT INTO itp (employee_id, position, qualification)
VALUES (v_employee_id, p_position, p_qualification);
INSERT INTO personnel_movement (employee_id, movement_type, date)
VALUES (v_employee_id, 'hire', p_hire_date);
END;
$$;
CREATE
OR REPLACE PROCEDURE proc_transfer_employee(
p_employee_id INTEGER,
p_new_brigade_id INTEGER DEFAULT NULL,
p_new_section_id INTEGER DEFAULT NULL,
p_description TEXT DEFAULT NULL
)
LANGUAGE plpgsql AS
$$
BEGIN
PERFORM
employee_id FROM employee
WHERE employee_id = p_employee_id
FOR
UPDATE;
IF
NOT FOUND THEN
RAISE EXCEPTION 'Employee % not found', p_employee_id;
END IF;
IF
EXISTS (SELECT 1 FROM worker WHERE employee_id = p_employee_id) THEN
UPDATE worker
SET brigade_id = p_new_brigade_id
WHERE employee_id = p_employee_id;
END IF;
IF
EXISTS (SELECT 1 FROM itp WHERE employee_id = p_employee_id) THEN
UPDATE itp
SET master_section_id = p_new_section_id
WHERE employee_id = p_employee_id;
END IF;
INSERT INTO personnel_movement (employee_id, movement_type, date, description)
VALUES (p_employee_id, 'transfer', CURRENT_DATE, p_description);
END;
$$;
CREATE
OR REPLACE PROCEDURE proc_dismiss_employee(
p_employee_id INTEGER,
p_description TEXT DEFAULT NULL
)
LANGUAGE plpgsql AS
$$
BEGIN
PERFORM
employee_id FROM employee
WHERE employee_id = p_employee_id
FOR
UPDATE;
IF
NOT FOUND THEN
RAISE EXCEPTION 'Employee % not found', p_employee_id;
END IF;
UPDATE worker
SET brigade_id = NULL
WHERE employee_id = p_employee_id;
UPDATE itp
SET master_section_id = NULL
WHERE employee_id = p_employee_id;
UPDATE brigade
SET foreman_id = NULL
WHERE foreman_id = p_employee_id;
UPDATE section
SET section_head_id = NULL
WHERE section_head_id = p_employee_id;
UPDATE workshop
SET head_id = NULL
WHERE head_id = p_employee_id;
INSERT INTO personnel_movement (employee_id, movement_type, date, description)
VALUES (p_employee_id, 'dismissal', CURRENT_DATE, p_description);
END;
$$;
CREATE
OR REPLACE PROCEDURE proc_assign_brigade_to_stage(
p_instance_id INTEGER,
p_stage_id INTEGER,
p_brigade_id INTEGER,
p_start_date DATE
)
LANGUAGE plpgsql AS
$$
BEGIN
PERFORM
instance_id FROM product_instance
WHERE instance_id = p_instance_id
FOR
UPDATE;
IF
NOT FOUND THEN
RAISE EXCEPTION 'Product instance % not found', p_instance_id;
END IF;
INSERT INTO assembly_record (stage_id, instance_id, brigade_id, start_date, status)
VALUES (p_stage_id, p_instance_id, p_brigade_id, p_start_date, 'in_progress');
END;
$$;
CREATE
OR REPLACE PROCEDURE proc_complete_stage(
p_record_id INTEGER,
p_end_date DATE
)
LANGUAGE plpgsql AS
$$
DECLARE
v_instance_id INTEGER;
BEGIN
UPDATE assembly_record
SET status   = 'completed',
    end_date = p_end_date
WHERE record_id = p_record_id RETURNING instance_id
INTO v_instance_id;
IF
NOT FOUND THEN
RAISE EXCEPTION 'Assembly record % not found', p_record_id;
END IF;
PERFORM
instance_id FROM product_instance
WHERE instance_id = v_instance_id
FOR
UPDATE;
END;
$$;
CREATE
OR REPLACE PROCEDURE proc_register_test(
p_instance_id INTEGER,
p_laboratory_id INTEGER,
p_test_date DATE
)
LANGUAGE plpgsql AS
$$
BEGIN
INSERT INTO test (instance_id, laboratory_id, test_date)
VALUES (p_instance_id, p_laboratory_id, p_test_date);
END;
$$;
CREATE
OR REPLACE PROCEDURE proc_complete_test(
p_test_id INTEGER,
p_result TEXT
)
LANGUAGE plpgsql AS
$$
DECLARE
v_instance_id   INTEGER;
v_pending_tests
INTEGER;
BEGIN
UPDATE test
SET result = p_result
WHERE test_id = p_test_id RETURNING instance_id
INTO v_instance_id;
IF
NOT FOUND THEN
RAISE EXCEPTION 'Test % not found', p_test_id;
END IF;
PERFORM
instance_id FROM product_instance
WHERE instance_id = v_instance_id
FOR
UPDATE;
SELECT COUNT(*)
INTO v_pending_tests
FROM test
WHERE instance_id = v_instance_id
  AND result IS NULL;
IF
v_pending_tests = 0 THEN
UPDATE product_instance
SET status = 'released'
WHERE instance_id = v_instance_id
  AND status = 'testing';
END IF;
END;
$$;
