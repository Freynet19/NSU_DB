CREATE TYPE employee_category_type AS ENUM ('ITP', 'Worker');
CREATE TYPE product_instance_status AS ENUM ( 'assembling', 'assembled', 'testing', 'released' );
CREATE TYPE assembly_record_status AS ENUM ('in_progress', 'completed');
CREATE TYPE personnel_movement_type AS ENUM ('hire', 'transfer', 'dismissal');
CREATE TABLE personnel_category
(
    category_code SERIAL PRIMARY KEY,
    name          VARCHAR(100)           NOT NULL CHECK (btrim(name) <> ''),
    type          employee_category_type NOT NULL
);
CREATE TABLE product_category
(
    category_id SERIAL PRIMARY KEY,
    name        VARCHAR(100) NOT NULL CHECK (btrim(name) <> '') UNIQUE
);
CREATE TABLE work_type
(
    work_type_id SERIAL PRIMARY KEY,
    name         VARCHAR(100) NOT NULL CHECK (btrim(name) <> '') UNIQUE,
    description  TEXT
);
CREATE TABLE laboratory
(
    laboratory_id SERIAL PRIMARY KEY,
    name          VARCHAR(100) NOT NULL CHECK (btrim(name) <> '') UNIQUE,
    location      VARCHAR(200)
);
CREATE TABLE workshop
(
    workshop_id SERIAL PRIMARY KEY,
    name        VARCHAR(100) NOT NULL CHECK (btrim(name) <> ''),
    head_id     INTEGER
);
CREATE TABLE employee
(
    employee_id   SERIAL PRIMARY KEY,
    full_name     VARCHAR(200) NOT NULL CHECK (btrim(full_name) <> ''),
    birth_date    DATE         NOT NULL,
    hire_date     DATE         NOT NULL,
    category_code INTEGER      NOT NULL REFERENCES personnel_category (category_code) ON DELETE RESTRICT
);
CREATE TABLE worker
(
    employee_id INTEGER PRIMARY KEY REFERENCES employee (employee_id) ON DELETE CASCADE,
    specialty   VARCHAR(100) NOT NULL CHECK (btrim(specialty) <> ''),
    grade       INTEGER      NOT NULL CHECK (grade BETWEEN 1 AND 8),
    brigade_id  INTEGER
);
CREATE TABLE itp
(
    employee_id       INTEGER PRIMARY KEY REFERENCES employee (employee_id) ON DELETE CASCADE,
    position          VARCHAR(100) NOT NULL CHECK (btrim(position) <> ''),
    qualification     VARCHAR(100),
    master_section_id INTEGER
);
CREATE TABLE section
(
    section_id      SERIAL PRIMARY KEY,
    name            VARCHAR(100) NOT NULL CHECK (btrim(name) <> ''),
    workshop_id     INTEGER      NOT NULL REFERENCES workshop (workshop_id) ON DELETE RESTRICT ON UPDATE CASCADE,
    section_head_id INTEGER      REFERENCES itp (employee_id) ON DELETE SET NULL
);
CREATE TABLE brigade
(
    brigade_id SERIAL PRIMARY KEY,
    name       VARCHAR(100) NOT NULL CHECK (btrim(name) <> ''),
    section_id INTEGER      NOT NULL REFERENCES section (section_id) ON DELETE RESTRICT ON UPDATE CASCADE,
    foreman_id INTEGER
);
ALTER TABLE worker
    ADD CONSTRAINT fk_worker_brigade FOREIGN KEY (brigade_id) REFERENCES brigade (brigade_id) ON DELETE SET NULL;
ALTER TABLE brigade
    ADD CONSTRAINT fk_foreman FOREIGN KEY (foreman_id) REFERENCES worker (employee_id) ON DELETE SET NULL;
ALTER TABLE workshop
    ADD CONSTRAINT fk_workshop_head FOREIGN KEY (head_id) REFERENCES itp (employee_id) ON DELETE SET NULL;
ALTER TABLE itp
    ADD CONSTRAINT fk_master_section FOREIGN KEY (master_section_id) REFERENCES section (section_id) ON DELETE SET NULL;
CREATE TABLE product_type
(
    type_id       SERIAL PRIMARY KEY,
    model_name    VARCHAR(100) NOT NULL CHECK (btrim(model_name) <> ''),
    category_id   INTEGER      NOT NULL REFERENCES product_category (category_id) ON DELETE RESTRICT,
    workshop_id   INTEGER      NOT NULL REFERENCES workshop (workshop_id) ON DELETE RESTRICT,
    capacity      INTEGER CHECK (capacity > 0),
    productivity  NUMERIC(10, 2) CHECK (productivity > 0),
    load_capacity NUMERIC(10, 2) CHECK (load_capacity > 0),
    engine_type   VARCHAR(50)
);
CREATE TABLE product_instance
(
    instance_id SERIAL PRIMARY KEY,
    type_id     INTEGER                 NOT NULL REFERENCES product_type (type_id) ON DELETE RESTRICT,
    start_date  DATE                    NOT NULL,
    end_date    DATE CHECK (end_date >= start_date),
    status      product_instance_status NOT NULL DEFAULT 'assembling'
);
CREATE TABLE production_cycle
(
    stage_id        SERIAL PRIMARY KEY,
    product_type_id INTEGER NOT NULL REFERENCES product_type (type_id) ON DELETE RESTRICT,
    work_type_id    INTEGER NOT NULL REFERENCES work_type (work_type_id) ON DELETE RESTRICT,
    section_id      INTEGER NOT NULL REFERENCES section (section_id) ON DELETE RESTRICT,
    stage_number    INTEGER NOT NULL CHECK (stage_number > 0),
    UNIQUE (product_type_id, stage_number)
);
CREATE TABLE assembly_record
(
    record_id   SERIAL PRIMARY KEY,
    stage_id    INTEGER                NOT NULL REFERENCES production_cycle (stage_id) ON DELETE RESTRICT,
    instance_id INTEGER                NOT NULL REFERENCES product_instance (instance_id) ON DELETE RESTRICT,
    brigade_id  INTEGER                NOT NULL REFERENCES brigade (brigade_id) ON DELETE RESTRICT,
    start_date  DATE                   NOT NULL,
    end_date    DATE CHECK (end_date >= start_date),
    status      assembly_record_status NOT NULL,
    UNIQUE (instance_id, stage_id)
);
CREATE UNIQUE INDEX uq_brigade_foreman ON brigade (foreman_id) WHERE foreman_id IS NOT NULL;
CREATE TABLE equipment
(
    equipment_id  SERIAL PRIMARY KEY,
    laboratory_id INTEGER      NOT NULL REFERENCES laboratory (laboratory_id) ON DELETE RESTRICT,
    name          VARCHAR(100) NOT NULL CHECK (btrim(name) <> ''),
    type          VARCHAR(100)
);
CREATE TABLE test
(
    test_id       SERIAL PRIMARY KEY,
    instance_id   INTEGER NOT NULL REFERENCES product_instance (instance_id) ON DELETE RESTRICT,
    laboratory_id INTEGER NOT NULL REFERENCES laboratory (laboratory_id) ON DELETE RESTRICT,
    test_date     DATE    NOT NULL,
    result        TEXT
);
CREATE TABLE personnel_movement
(
    record_id     SERIAL PRIMARY KEY,
    employee_id   INTEGER                 NOT NULL REFERENCES employee (employee_id) ON DELETE CASCADE,
    movement_type personnel_movement_type NOT NULL,
    date          DATE                    NOT NULL,
    description   TEXT
);
CREATE TABLE workshop_laboratory
(
    workshop_id   INTEGER NOT NULL REFERENCES workshop (workshop_id) ON DELETE CASCADE,
    laboratory_id INTEGER NOT NULL REFERENCES laboratory (laboratory_id) ON DELETE CASCADE,
    PRIMARY KEY (workshop_id, laboratory_id)
);
CREATE TABLE test_specialist
(
    test_id     INTEGER NOT NULL REFERENCES test (test_id) ON DELETE CASCADE,
    employee_id INTEGER NOT NULL REFERENCES itp (employee_id) ON DELETE CASCADE,
    PRIMARY KEY (test_id, employee_id)
);
CREATE TABLE test_equipment
(
    test_id      INTEGER NOT NULL REFERENCES test (test_id) ON DELETE CASCADE,
    equipment_id INTEGER NOT NULL REFERENCES equipment (equipment_id) ON DELETE CASCADE,
    PRIMARY KEY (test_id, equipment_id)
);
