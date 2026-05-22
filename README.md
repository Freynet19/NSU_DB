# ИС автомобилестроительного предприятия (NSU_DB)

Клиент-серверное приложение: **Qt6/C++20** (десктоп) + **PostgreSQL 18** (Docker).

## Архитектура

- **Сервер данных:** PostgreSQL 18 в Docker (`docker-compose.yml`). Отдельный C++ API-сервер не используется.
- **Клиент:** `client/` — прямое подключение по QPSQL под учётной записью роли (`user_admin`, `user_hr`, `user_production`).
- **Слои клиента:** `DatabaseManager` (подключение, загрузка `PREPARE`), `QueryRepository` / `LookupRepository`, интерфейс по ролям (`AdminWidget`, `HrWidget`, `ProductionReportWidget`).
- **Конкурентный доступ:** в процедурах — `SELECT … FOR UPDATE` перед изменением строк; у ролей `role_admin`, `role_hr`, `role_production_report` — `lock_timeout` 5 с, `statement_timeout` 30 с. CRUD администратора — отложенное сохранение (`OnManualSubmit`).

## Быстрый старт (WSL Ubuntu)

### 1. Зависимости

```bash
sudo apt update
sudo apt install -y docker.io docker-compose-v2 \
    cmake g++ qt6-base-dev qt6-base-dev-tools libqt6-sql6-psql libpq-dev
```

Добавьте пользователя в группу `docker` и перелогиньтесь, если Docker требует `sudo`.

### 2. База данных

```bash
cd /path/to/NSU_DB
docker compose up -d
```

Скрипты из `db/init/` применяются **один раз** при первом создании volume. Полная переустановка:

```bash
docker compose down -v
docker compose up -d
```

Проверка:

```bash
docker exec nsu_db_postgres psql -U user_admin -d test -c "SELECT COUNT(*) FROM workshop;"
```

### 3. Сборка клиента

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/client/automotive_erp_client
```

Для CLion: откройте корневой `CMakeLists.txt`, укажите toolchain WSL (каталог сборки по умолчанию — `cmake-build-debug/`).

### 4. Вход в приложение

| Пользователь       | Пароль               | Роль в UI              |
|--------------------|----------------------|------------------------|
| `user_admin`       | `admin_password`     | Администратор (полный доступ) |
| `user_hr`          | `hr_password`        | Кадровик               |
| `user_production`  | `production_password`| Производственная отчётность |

Параметры подключения по умолчанию: `localhost:5432`, БД `test`.

В БД также созданы `user_workshop` и `user_laboratory` (см. `04_roles_users.sql`); в текущем клиенте в диалоге входа не используются.

Справочник переменных окружения (для справки, клиент читает настройки из UI): `.env.example`.

## SQL-скрипты

| Файл | Содержимое |
|------|------------|
| `db/init/00_types_tables.sql` | Типы, таблицы, индексы |
| `db/init/01_triggers.sql` | Триггеры |
| `db/init/02_procedures.sql` | 8 хранимых процедур |
| `db/init/03_views.sql` | 12 представлений (`vw_*`) |
| `db/init/04_roles_users.sql` | Роли и пользователи |
| `db/init/05_grants_extend.sql` | Права HR и production для Qt-клиента |
| `db/init/06_seed.sql` | Тестовые данные |

**PREPARE** для отчётов не в init: [`client/resources/prepare_statements.sql`](client/resources/prepare_statements.sql) выполняется при входе в приложение (на сессию).

Исходный текст также в отчёте: `Отчёт 23201 Смирнов версия 0.6.md`.

## Функциональность клиента

- **Администратор:** CRUD всех таблиц (вкладка «Справочники»), `CALL` всех 8 процедур, все 14 отчётов (`EXECUTE` подготовленных запросов по `vw_*`).
- **Кадровик (HR):** процедуры `proc_hire_worker`, `proc_hire_itp`, `proc_transfer_employee`, `proc_dismiss_employee`; отчёты **3, 4, 6, 7** (кадровый состав — фильтры: цех, категория персонала, тип; участки; состав бригад; мастера участков).
- **Производственная отчётность:** только чтение — отчёты **1, 2, 8, 10, 11, 14** (виды изделий; собранные за период; собираемые сейчас; лаборатории изделия; испытанные в лаборатории — лаборатория обязательна; список/число собираемых).

При подключении клиент выполняет `PREPARE` из `prepare_statements.sql` в текущей сессии; тела запросов читают данные через представления `vw_*` (права ролей — `GRANT SELECT` в `04_roles_users.sql` / `05_grants_extend.sql`).

После успешных процедур и сохранения справочников обновляются выпадающие списки (сброс кэша `LookupRepository`).

## Структура репозитория

```
NSU_DB/
├── .env.example
├── .gitignore
├── CMakeLists.txt
├── README.md
├── docker-compose.yml
├── client/
│   ├── CMakeLists.txt
│   ├── resources/
│   │   ├── app.qrc
│   │   └── prepare_statements.sql
│   ├── src/
│   │   ├── main.cpp
│   │   ├── auth/
│   │   ├── core/
│   │   ├── crud/
│   │   ├── db/
│   │   ├── procedures/
│   │   ├── reports/
│   │   ├── roles/
│   │   ├── shell/
│   │   └── widgets/
│   └── ui/
└── db/init/
    ├── 00_types_tables.sql
    ├── 01_triggers.sql
    ├── 02_procedures.sql
    ├── 03_views.sql
    ├── 04_roles_users.sql
    ├── 05_grants_extend.sql
    └── 06_seed.sql
```
