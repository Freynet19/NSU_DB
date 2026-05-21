CREATE
OR REPLACE FUNCTION fn_check_worker_subtype()
RETURNS TRIGGER AS $$
BEGIN
PERFORM
employee_id FROM employee
WHERE employee_id = NEW.employee_id
FOR
UPDATE;
PERFORM
employee_id FROM itp
WHERE employee_id = NEW.employee_id;
IF
FOUND THEN
RAISE EXCEPTION 'Employee % is already registered as ITP', NEW.employee_id;
END IF;
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE
OR REPLACE FUNCTION fn_check_itp_subtype()
RETURNS TRIGGER AS $$
BEGIN
PERFORM
employee_id FROM employee
WHERE employee_id = NEW.employee_id
FOR
UPDATE;
PERFORM
employee_id FROM worker
WHERE employee_id = NEW.employee_id;
IF
FOUND THEN
RAISE EXCEPTION 'Employee % is already registered as Worker', NEW.employee_id;
END IF;
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE TRIGGER trg_check_worker_subtype
    BEFORE INSERT
    ON worker
    FOR EACH ROW EXECUTE FUNCTION fn_check_worker_subtype();
CREATE TRIGGER trg_check_itp_subtype
    BEFORE INSERT
    ON itp
    FOR EACH ROW EXECUTE FUNCTION fn_check_itp_subtype();
CREATE
OR REPLACE FUNCTION fn_check_brigadier()
RETURNS TRIGGER AS $$
BEGIN
IF
NEW.foreman_id IS NOT NULL THEN
PERFORM brigade_id FROM brigade
WHERE foreman_id = NEW.foreman_id
AND brigade_id IS DISTINCT FROM NEW.brigade_id
FOR
UPDATE;
IF
FOUND THEN
RAISE EXCEPTION 'Worker % is already a foreman of another brigade', NEW.foreman_id;
END IF;
END IF;
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE TRIGGER trg_check_brigadier
    BEFORE INSERT OR
UPDATE ON brigade
    FOR EACH ROW EXECUTE FUNCTION fn_check_brigadier();
CREATE
OR REPLACE FUNCTION fn_check_section_head()
RETURNS TRIGGER AS $$
BEGIN
IF
NEW.section_head_id IS NOT NULL AND NOT EXISTS (
SELECT 1 FROM itp WHERE employee_id = NEW.section_head_id
) THEN
RAISE EXCEPTION 'Employee % is not registered as ITP', NEW.section_head_id;
END IF;
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE TRIGGER trg_check_section_head
    BEFORE INSERT OR
UPDATE ON section
    FOR EACH ROW EXECUTE FUNCTION fn_check_section_head();
CREATE
OR REPLACE FUNCTION fn_check_workshop_head()
RETURNS TRIGGER AS $$
BEGIN
IF
NEW.head_id IS NOT NULL AND NOT EXISTS (
SELECT 1 FROM itp WHERE employee_id = NEW.head_id
) THEN
RAISE EXCEPTION 'Employee % is not registered as ITP', NEW.head_id;
END IF;
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE TRIGGER trg_check_workshop_head
    BEFORE INSERT OR
UPDATE ON workshop
    FOR EACH ROW EXECUTE FUNCTION fn_check_workshop_head();
CREATE
OR REPLACE FUNCTION fn_check_brigade_section()
RETURNS TRIGGER AS $$
DECLARE
v_stage_section_id   INTEGER;
v_brigade_section_id
INTEGER;
BEGIN
SELECT section_id
INTO v_stage_section_id
FROM production_cycle
WHERE stage_id = NEW.stage_id
    FOR SHARE;
SELECT section_id
INTO v_brigade_section_id
FROM brigade
WHERE brigade_id = NEW.brigade_id
    FOR SHARE;
IF
v_stage_section_id != v_brigade_section_id THEN
RAISE EXCEPTION 'Brigade % does not belong to section % required by stage %',
NEW.brigade_id, v_stage_section_id, NEW.stage_id;
END IF;
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE TRIGGER trg_check_brigade_section
    BEFORE INSERT OR
UPDATE ON assembly_record
    FOR EACH ROW EXECUTE FUNCTION fn_check_brigade_section();
CREATE
OR REPLACE FUNCTION fn_check_equipment_lab()
RETURNS TRIGGER AS $$
DECLARE
v_test_lab_id      INTEGER;
v_equipment_lab_id
INTEGER;
BEGIN
SELECT laboratory_id
INTO v_test_lab_id
FROM test
WHERE test_id = NEW.test_id
    FOR SHARE;
SELECT laboratory_id
INTO v_equipment_lab_id
FROM equipment
WHERE equipment_id = NEW.equipment_id
    FOR SHARE;
IF
v_test_lab_id != v_equipment_lab_id THEN
RAISE EXCEPTION 'Equipment % belongs to laboratory %, but test % is in laboratory %',
NEW.equipment_id, v_equipment_lab_id, NEW.test_id, v_test_lab_id;
END IF;
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE TRIGGER trg_check_equipment_lab
    BEFORE INSERT OR
UPDATE ON test_equipment
    FOR EACH ROW EXECUTE FUNCTION fn_check_equipment_lab();
CREATE
OR REPLACE FUNCTION fn_update_product_status()
RETURNS TRIGGER AS $$
DECLARE
v_total_stages     INTEGER;
v_completed_stages
INTEGER;
BEGIN
PERFORM
instance_id FROM product_instance
WHERE instance_id = NEW.instance_id
FOR
UPDATE;
SELECT COUNT(*)
INTO v_total_stages
FROM production_cycle pc
         JOIN product_instance pi ON pi.type_id = pc.product_type_id
WHERE pi.instance_id = NEW.instance_id;
SELECT COUNT(*)
INTO v_completed_stages
FROM assembly_record
WHERE instance_id = NEW.instance_id
  AND status = 'completed';
IF
v_total_stages > 0 AND v_completed_stages >= v_total_stages THEN
UPDATE product_instance
SET status   = 'assembled',
    end_date = CURRENT_DATE
WHERE instance_id = NEW.instance_id
  AND status = 'assembling';
ELSIF
v_total_stages > 0 AND v_completed_stages < v_total_stages THEN
UPDATE product_instance
SET status = 'assembling'
WHERE instance_id = NEW.instance_id
  AND status NOT IN ('testing', 'released');
END IF;
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE TRIGGER trg_update_product_status
    AFTER INSERT OR
UPDATE ON assembly_record
    FOR EACH ROW EXECUTE FUNCTION fn_update_product_status();
CREATE
OR REPLACE FUNCTION fn_check_product_status_for_test()
RETURNS TRIGGER AS $$
DECLARE
v_status product_instance_status;
BEGIN
SELECT status
INTO v_status
FROM product_instance
WHERE instance_id = NEW.instance_id
    FOR UPDATE;
IF
v_status NOT IN ('assembled', 'testing') THEN
RAISE EXCEPTION 'Product instance % has status %, expected assembled or testing',
NEW.instance_id, v_status;
END IF;
UPDATE product_instance
SET status = 'testing'
WHERE instance_id = NEW.instance_id
  AND status = 'assembled';
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE TRIGGER trg_check_product_status_for_test
    BEFORE INSERT
    ON test
    FOR EACH ROW EXECUTE FUNCTION fn_check_product_status_for_test();
CREATE
OR REPLACE FUNCTION fn_check_product_type_attrs()
RETURNS TRIGGER AS $$
DECLARE
v_category_name VARCHAR(100);
BEGIN
SELECT name
INTO v_category_name
FROM product_category
WHERE category_id = NEW.category_id;
IF
v_category_name = 'Автобусы' AND NEW.capacity IS NULL THEN
RAISE EXCEPTION 'Field capacity is required for category Автобусы';
END IF;
IF
v_category_name IN ('Сельскохозяйственные машины', 'Дорожно-строительные машины')
AND NEW.productivity IS NULL THEN
RAISE EXCEPTION 'Field productivity is required for category %', v_category_name;
END IF;
IF
v_category_name = 'Грузовые автомобили' THEN
IF NEW.load_capacity IS NULL THEN
RAISE EXCEPTION 'Field load_capacity is required for category Грузовые автомобили';
END IF;
IF
NEW.engine_type IS NULL THEN
RAISE EXCEPTION 'Field engine_type is required for category Грузовые автомобили';
END IF;
END IF;
IF
v_category_name IN ('Легковые автомобили', 'Мотоциклы') AND NEW.engine_type IS NULL THEN
RAISE EXCEPTION 'Field engine_type is required for category %', v_category_name;
END IF;
RETURN NEW;
END;
$$
LANGUAGE plpgsql;
CREATE TRIGGER trg_check_product_type_attrs
    BEFORE INSERT OR
UPDATE ON product_type
    FOR EACH ROW EXECUTE FUNCTION fn_check_product_type_attrs();
