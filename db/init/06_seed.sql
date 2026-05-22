BEGIN;
INSERT INTO personnel_category (category_code, name, type)
VALUES (1, 'Слесарь-сборщик', 'Worker'),
       (2, 'Сварщик', 'Worker'),
       (3, 'Автоэлектрик', 'Worker'),
       (4, 'Монтажник гидросистем', 'Worker'),
       (5, 'Мотослесарь', 'Worker'),
       (6, 'Начальник цеха', 'ITP'),
       (7, 'Мастер участка', 'ITP'),
       (8, 'Инженер-испытатель', 'ITP');
INSERT INTO product_category (category_id, name)
VALUES (1, 'Автобусы'),
       (2, 'Сельскохозяйственные машины'),
       (3, 'Дорожно-строительные машины'),
       (4, 'Грузовые автомобили'),
       (5, 'Легковые автомобили'),
       (6, 'Мотоциклы');
INSERT INTO work_type (work_type_id, name, description)
VALUES (1, 'Сварка несущих конструкций', 'Сварка рам, каркасов и несущих металлических элементов изделия'),
       (2, 'Механическая сборка', 'Сборка основных механических узлов, агрегатов и силовых элементов'),
       (3, 'Монтаж гидросистем', 'Установка гидроцилиндров, магистралей и насосного оборудования'),
       (4, 'Электромонтаж', 'Монтаж жгутов проводки, электронных блоков и систем освещения'),
       (5, 'Финальная сборка', 'Окончательная сборка изделия с установкой комплектующих и регулировкой'),
       (6, 'Окраска и антикоррозийная обработка', 'Подготовка поверхностей, окраска и защитная обработка');
INSERT INTO laboratory (laboratory_id, name, location)
VALUES (1, 'Лаборатория двигательных испытаний', 'Корпус 2, испытательный блок'),
       (2, 'Лаборатория электрооборудования', 'Корпус 3, 2 этаж'),
       (3, 'Лаборатория тормозных систем', 'Корпус 2, секция Б'),
       (4, 'Гидравлическая лаборатория', 'Корпус 4, стендовый зал'),
       (5, 'Лаборатория ресурсных испытаний', 'Полигон №1');
INSERT INTO workshop (workshop_id, name, head_id)
VALUES (1, 'Автобусный цех', NULL),
       (2, 'Цех грузовых автомобилей', NULL),
       (3, 'Цех специальной техники', NULL),
       (4, 'Цех легковых автомобилей', NULL),
       (5, 'Мотоциклетный цех', NULL);
INSERT INTO section (section_id, name, workshop_id, section_head_id)
VALUES (1, 'Участок сварки автобусных кузовов', 1, NULL),
       (2, 'Участок финальной сборки автобусов', 1, NULL),
       (3, 'Участок сборки грузовых шасси', 2, NULL),
       (4, 'Участок сборки специальной техники', 3, NULL),
       (5, 'Участок финальной сборки легковых автомобилей', 4, NULL),
       (6, 'Участок мотоциклетной сборки', 5, NULL);
INSERT INTO brigade (brigade_id, name, section_id, foreman_id)
VALUES (1, 'Бригада автобусных кузовов', 1, NULL),
       (2, 'Бригада автобусной сборки', 2, NULL),
       (3, 'Бригада грузовых шасси', 3, NULL),
       (4, 'Бригада специальной техники', 4, NULL),
       (5, 'Бригада легковой сборки', 5, NULL),
       (6, 'Бригада мотоциклетной сборки', 6, NULL);
INSERT INTO employee (employee_id, full_name, birth_date, hire_date, category_code)
VALUES (1, 'Петров Алексей Сергеевич', '1980-04-12', '2014-03-17', 6),
       (2, 'Власов Игорь Николаевич', '1978-09-25', '2012-06-04', 6),
       (3, 'Беляева Марина Олеговна', '1984-01-18', '2016-09-12', 6),
       (4, 'Крылов Дмитрий Павлович', '1986-07-08', '2017-02-20', 6),
       (5, 'Соколова Анна Викторовна', '1988-11-30', '2018-05-14', 6),
       (6, 'Громов Сергей Евгеньевич', '1987-03-22', '2019-01-10', 7),
       (7, 'Орлова Наталья Игоревна', '1990-08-05', '2020-10-01', 8),
       (8, 'Кузнецов Павел Ильич', '1991-05-19', '2021-04-05', 2),
       (9, 'Морозов Андрей Владимирович', '1993-02-14', '2022-07-11', 1),
       (10, 'Федоров Николай Степанович', '1989-12-03', '2021-09-27', 1),
       (11, 'Кириллов Олег Андреевич', '1992-06-28', '2023-01-16', 4),
       (12, 'Павлова Ирина Сергеевна', '1994-10-07', '2023-03-13', 3),
       (13, 'Никитин Артем Романович', '1995-01-24', '2024-02-19', 5);
INSERT INTO itp (employee_id, position, qualification, master_section_id)
VALUES (1, 'Начальник автобусного цеха', 'Высшая категория', 2),
       (2, 'Начальник цеха грузовых автомобилей', 'Высшая категория', 3),
       (3, 'Начальник цеха специальной техники', 'Первая категория', 4),
       (4, 'Начальник цеха легковых автомобилей', 'Первая категория', 5),
       (5, 'Начальник мотоциклетного цеха', 'Первая категория', 6),
       (6, 'Мастер участка сборки', 'Первая категория', 1),
       (7, 'Инженер-испытатель', 'Высшая категория', NULL);
INSERT INTO worker (employee_id, specialty, grade, brigade_id)
VALUES (8, 'Сварка кузовных металлоконструкций', 6, 1),
       (9, 'Слесарная сборка кузова и салона', 5, 2),
       (10, 'Сборка грузовых шасси', 6, 3),
       (11, 'Монтаж гидравлических систем', 5, 4),
       (12, 'Автоэлектромонтаж', 6, 5),
       (13, 'Сборка мотоциклетных узлов', 5, 6);
UPDATE workshop
SET head_id = CASE workshop_id
                  WHEN 1 THEN 1
                  WHEN 2 THEN 2
                  WHEN 3 THEN 3
                  WHEN 4 THEN 4
                  WHEN 5 THEN 5
    END
WHERE workshop_id BETWEEN 1 AND 5;
UPDATE section
SET section_head_id = CASE section_id
                          WHEN 1 THEN 6
                          WHEN 2 THEN 1
                          WHEN 3 THEN 2
                          WHEN 4 THEN 3
                          WHEN 5 THEN 4
                          WHEN 6 THEN 5
    END
WHERE section_id BETWEEN 1 AND 6;
UPDATE brigade
SET foreman_id = CASE brigade_id
                     WHEN 1 THEN 8
                     WHEN 2 THEN 9
                     WHEN 3 THEN 10
                     WHEN 4 THEN 11
                     WHEN 5 THEN 12
                     WHEN 6 THEN 13
    END
WHERE brigade_id BETWEEN 1 AND 6;
INSERT INTO product_type
(type_id, model_name, category_id, workshop_id, capacity, productivity, load_capacity, engine_type)
VALUES (1, 'VolgaBus CityMax 12', 1, 1, 105, NULL, NULL, 'дизельный'),
       (2, 'AgroMaster 3200', 2, 3, NULL, 18.50, NULL, 'дизельный'),
       (3, 'RoadPro RM-240', 3, 3, NULL, 12.00, NULL, 'дизельный'),
       (4, 'Titan 18T', 4, 2, NULL, NULL, 18.00, 'дизельный'),
       (5, 'Ladoga Sedan 1.6', 5, 4, NULL, NULL, NULL, 'бензиновый'),
       (6, 'Meteor 300', 6, 5, NULL, NULL, NULL, 'бензиновый');
INSERT INTO product_instance (instance_id, type_id, start_date, end_date, status)
VALUES (1, 1, '2026-01-15', '2026-02-28', 'assembled'),
       (2, 2, '2026-02-01', '2026-03-05', 'assembled'),
       (3, 3, '2026-01-25', '2026-03-10', 'assembled'),
       (4, 4, '2026-02-10', '2026-03-15', 'assembled'),
       (5, 5, '2026-02-20', '2026-03-18', 'assembled'),
       (6, 6, '2026-03-01', '2026-03-22', 'assembled');
INSERT INTO production_cycle (stage_id, product_type_id, work_type_id, section_id, stage_number)
VALUES (1, 1, 1, 1, 1),
       (2, 1, 5, 2, 2),
       (3, 2, 2, 4, 1),
       (4, 2, 3, 4, 2),
       (5, 3, 2, 4, 1),
       (6, 3, 6, 4, 2),
       (7, 4, 2, 3, 1),
       (8, 4, 5, 3, 2),
       (9, 5, 4, 5, 1),
       (10, 5, 5, 5, 2),
       (11, 6, 2, 6, 1),
       (12, 6, 5, 6, 2);
INSERT INTO assembly_record (record_id, stage_id, instance_id, brigade_id, start_date, end_date, status)
VALUES (1, 1, 1, 1, '2026-01-16', '2026-02-05', 'completed'),
       (2, 2, 1, 2, '2026-02-06', '2026-02-28', 'completed'),
       (3, 3, 2, 4, '2026-02-02', '2026-02-20', 'completed'),
       (4, 4, 2, 4, '2026-02-21', '2026-03-05', 'completed'),
       (5, 5, 3, 4, '2026-01-26', '2026-02-20', 'completed'),
       (6, 6, 3, 4, '2026-02-21', '2026-03-10', 'completed'),
       (7, 7, 4, 3, '2026-02-11', '2026-02-28', 'completed'),
       (8, 8, 4, 3, '2026-03-01', '2026-03-15', 'completed'),
       (9, 9, 5, 5, '2026-02-21', '2026-03-05', 'completed'),
       (10, 10, 5, 5, '2026-03-06', '2026-03-18', 'completed'),
       (11, 11, 6, 6, '2026-03-02', '2026-03-12', 'completed'),
       (12, 12, 6, 6, '2026-03-13', '2026-03-22', 'completed');
-- Изделия в процессе сборки (отчёты 8 и 14: vw_current_products)
INSERT INTO product_instance (instance_id, type_id, start_date, end_date, status)
VALUES (7, 1, '2026-04-01', NULL, 'assembling'),
       (8, 4, '2026-04-05', NULL, 'assembling'),
       (9, 5, '2026-04-08', NULL, 'assembling'),
       (10, 2, '2026-04-10', NULL, 'assembling'),
       (11, 6, '2026-04-12', NULL, 'assembling');
INSERT INTO assembly_record (record_id, stage_id, instance_id, brigade_id, start_date, end_date, status)
VALUES (13, 1, 7, 1, '2026-04-02', NULL, 'in_progress'),
       (14, 7, 8, 3, '2026-04-06', NULL, 'in_progress'),
       (15, 9, 9, 5, '2026-04-09', NULL, 'in_progress'),
       (16, 3, 10, 4, '2026-04-11', NULL, 'in_progress'),
       (17, 11, 11, 6, '2026-04-13', NULL, 'in_progress');
INSERT INTO equipment (equipment_id, laboratory_id, name, type)
VALUES (1, 1, 'Динамометрический стенд AVL-250', 'Испытательный стенд'),
       (2, 1, 'Газоанализатор Инфракар 08.01', 'Измерительный прибор'),
       (3, 2, 'Стенд проверки жгутов E-Check 400', 'Диагностический стенд'),
       (4, 2, 'Осциллограф Rigol DS1202Z-E', 'Измерительный прибор'),
       (5, 3, 'Тормозной стенд СТМ-1500', 'Испытательный стенд'),
       (6, 4, 'Гидростенд ГС-90', 'Испытательный стенд'),
       (7, 5, 'Стенд ресурсных испытаний РС-500', 'Испытательный комплекс');
INSERT INTO workshop_laboratory (workshop_id, laboratory_id)
VALUES (1, 1),
       (1, 3),
       (2, 1),
       (3, 4),
       (3, 5),
       (4, 2),
       (5, 3);
INSERT INTO test (test_id, instance_id, laboratory_id, test_date, result)
VALUES (1, 1, 3, '2026-03-03', NULL),
       (2, 3, 5, '2026-03-12', NULL),
       (3, 4, 1, '2026-03-18', NULL),
       (4, 5, 2, '2026-03-20', NULL),
       (5, 6, 3, '2026-03-23', NULL);
INSERT INTO test_specialist (test_id, employee_id)
VALUES (1, 7),
       (2, 3),
       (3, 2),
       (4, 4),
       (5, 5);
INSERT INTO test_equipment (test_id, equipment_id)
VALUES (1, 5),
       (2, 7),
       (3, 1),
       (4, 3),
       (5, 5);
CALL proc_complete_test(1, 'Эффективность торможения соответствует техническим требованиям.');
CALL proc_complete_test(2, 'Ресурсные испытания завершены без отказов и критических замечаний.');
CALL proc_complete_test(3, 'Двигатель развивает номинальную мощность, отклонений не выявлено.');
CALL proc_complete_test(4, 'Электрооборудование работает стабильно, ошибок не обнаружено.');
CALL proc_complete_test(5, 'Тормозная система мотоцикла функционирует в пределах нормативов.');
INSERT INTO personnel_movement (record_id, employee_id, movement_type, date, description)
VALUES (1, 1, 'hire', '2014-03-17', 'Принят на должность начальника автобусного цеха'),
       (2, 2, 'hire', '2012-06-04', 'Принят на должность начальника цеха грузовых автомобилей'),
       (3, 3, 'hire', '2016-09-12', 'Принята на должность начальника цеха специальной техники'),
       (4, 4, 'hire', '2017-02-20', 'Принят на должность начальника цеха легковых автомобилей'),
       (5, 5, 'hire', '2018-05-14', 'Принята на должность начальника мотоциклетного цеха'),
       (6, 6, 'hire', '2019-01-10', 'Принят на должность мастера участка сборки'),
       (7, 7, 'hire', '2020-10-01', 'Принята на должность инженера-испытателя'),
       (8, 8, 'hire', '2021-04-05', 'Принят в бригаду автобусных кузовов'),
       (9, 9, 'hire', '2022-07-11', 'Принят в бригаду автобусной сборки'),
       (10, 10, 'hire', '2021-09-27', 'Принят в бригаду грузовых шасси'),
       (11, 11, 'hire', '2023-01-16', 'Принят в бригаду специальной техники'),
       (12, 12, 'hire', '2023-03-13', 'Принята в бригаду легковой сборки'),
       (13, 13, 'hire', '2024-02-19', 'Принят в бригаду мотоциклетной сборки');
-- После явных INSERT с id — выставить SERIAL, иначе следующий INSERT возьмёт 1 и даст duplicate key
SELECT setval(pg_get_serial_sequence('personnel_category', 'category_code'),
              COALESCE((SELECT MAX(category_code) FROM personnel_category), 1));
SELECT setval(pg_get_serial_sequence('product_category', 'category_id'),
              COALESCE((SELECT MAX(category_id) FROM product_category), 1));
SELECT setval(pg_get_serial_sequence('work_type', 'work_type_id'),
              COALESCE((SELECT MAX(work_type_id) FROM work_type), 1));
SELECT setval(pg_get_serial_sequence('laboratory', 'laboratory_id'),
              COALESCE((SELECT MAX(laboratory_id) FROM laboratory), 1));
SELECT setval(pg_get_serial_sequence('workshop', 'workshop_id'),
              COALESCE((SELECT MAX(workshop_id) FROM workshop), 1));
SELECT setval(pg_get_serial_sequence('section', 'section_id'),
              COALESCE((SELECT MAX(section_id) FROM section), 1));
SELECT setval(pg_get_serial_sequence('brigade', 'brigade_id'),
              COALESCE((SELECT MAX(brigade_id) FROM brigade), 1));
SELECT setval(pg_get_serial_sequence('employee', 'employee_id'),
              COALESCE((SELECT MAX(employee_id) FROM employee), 1));
SELECT setval(pg_get_serial_sequence('product_type', 'type_id'),
              COALESCE((SELECT MAX(type_id) FROM product_type), 1));
SELECT setval(pg_get_serial_sequence('product_instance', 'instance_id'),
              COALESCE((SELECT MAX(instance_id) FROM product_instance), 1));
SELECT setval(pg_get_serial_sequence('production_cycle', 'stage_id'),
              COALESCE((SELECT MAX(stage_id) FROM production_cycle), 1));
SELECT setval(pg_get_serial_sequence('assembly_record', 'record_id'),
              COALESCE((SELECT MAX(record_id) FROM assembly_record), 1));
SELECT setval(pg_get_serial_sequence('equipment', 'equipment_id'),
              COALESCE((SELECT MAX(equipment_id) FROM equipment), 1));
SELECT setval(pg_get_serial_sequence('test', 'test_id'),
              COALESCE((SELECT MAX(test_id) FROM test), 1));
SELECT setval(pg_get_serial_sequence('personnel_movement', 'record_id'),
              COALESCE((SELECT MAX(record_id) FROM personnel_movement), 1));
COMMIT;
