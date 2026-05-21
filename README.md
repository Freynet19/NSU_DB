# ИС автомобилестроительного предприятия (NSU_DB)

Клиент-серверное приложение: **Qt6/C++20** (десктоп) + **PostgreSQL 18** (Docker).

## Архитектура

- **Сервер данных:** PostgreSQL 18 в Docker (`docker-compose.yml`). Отдельный C++ API-сервер не используется.
- **Клиент:** `client/` — подключается по QPSQL к БД под учётной записью роли (`user_admin`, `user_hr`, `user_production`).

## Быстрый старт (WSL Ubuntu)

### 1. Зависимости

```bash
sudo apt update
sudo apt install -y docker.io docker-compose-v2 \
    cmake g++ qt6-base-dev qt6-base-dev-tools libqt6sql6-psql libpq-dev
```

Добавьте пользователя в группу `docker` и перелогиньтесь, если Docker требует `sudo`.

### 2. База данных

```bash
cd /home/freynet/clion/NSU_DB
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

Для CLion: откройте корневой `CMakeLists.txt`, укажите toolchain WSL.

### 4. Вход в приложение

| Пользователь       | Пароль               | Роль в UI              |
|--------------------|----------------------|------------------------|
| `user_admin`       | `admin_password`     | Администратор (полный доступ) |
| `user_hr`          | `hr_password`        | Кадровик               |
| `user_production`  | `production_password`| Производственная отчётность |

Параметры подключения по умолчанию: `localhost:5432`, БД `test`.

## SQL-скрипты

| Файл | Содержимое |
|------|------------|
| `db/init/00_types_tables.sql` | Типы, таблицы, индексы |
| `db/init/01_triggers.sql` | Триггеры |
| `db/init/02_procedures.sql` | 8 хранимых процедур |
| `db/init/03_views.sql` | 11 представлений |
| `db/init/04_roles_users.sql` | Роли и пользователи |
| `db/init/05_grants_extend.sql` | Доп. права HR и отчётности |
| `db/init/06_prepare.sql` | 14 PREPARE + примеры EXECUTE |
| `db/init/06_prepare_only.sql` | Только PREPARE (для сессии клиента) |
| `db/init/07_seed.sql` | Тестовые данные |

Исходный текст также в отчёте: `Отчёт 23201 Смирнов версия 0.6.md`.

## Функциональность клиента

- **Администратор:** CRUD всех таблиц, `CALL` всех процедур, 14 отчётов (`EXECUTE` PREPARE).
- **HR:** `proc_hire_worker`, `proc_hire_itp`, `proc_transfer_employee`, `proc_dismiss_employee`; отчёты 3, 4, 6, 7.
- **Отчётность:** только чтение — отчёты 1, 2, 8, 10, 11, 14.

При подключении клиент выполняет `PREPARE` из `prepare_statements.sql` в текущей сессии (в PostgreSQL prepared statements не сохраняются между сессиями).

## Структура репозитория

```
NSU_DB/
├── docker-compose.yml
├── db/init/
├── client/
├── build/          # каталог сборки (не в git)
└── Отчёт 23201 Смирнов версия 0.6.md
```
