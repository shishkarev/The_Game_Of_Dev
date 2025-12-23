# The_Game_Of_Dev

Проект для тестового задания.

Сборка даёт несколько бинарников:
- `The_Game_Of_Dev` — hello world
- `echo_server` — HTTP сервер (`/echo`, `/calc`, `/cmd`)
- `calc_cli` — локальный CLI калькулятор (парсит и считает без сети)
- `calc` — CLI HTTP-клиент (отправляет выражение на сервер)

---

## Build & Run (local)

```bash
mkdir -p build
cmake -S . -B build
cmake --build build
```

### Hello

```bash
./build/The_Game_Of_Dev
```

### HTTP server

Запуск:

```bash
./build/echo_server
```

Тест echo:

```bash
curl -i -X POST http://localhost:8080/echo -d "hi"
```

Тест калькулятора (HTTP) — сервер возвращает JSON:

```bash
curl -i -X POST http://localhost:8080/calc -d "2*(3+4)"
curl -i -X POST http://localhost:8080/calc -d "a=2+3; a*10"
curl -i -X POST http://localhost:8080/calc -d "10/0"
```

Пример успешного ответа:
```json
{"result":18.84}
```

Пример ошибки:
```json
{"error":"Division by zero"}
```

---

## Calculator language (Level 9)

Поддерживается мини-язык:

- арифметика: `+ - * /`
- скобки: `( )`
- унарные `+` и `-`
- переменные: `name = expr`
- несколько выражений/операторов через `;`
- результат программы — значение **последнего** выражения/оператора

Примеры:

```bash
# простая арифметика
curl -i -X POST http://localhost:8080/calc -d "2*(3+4)"

# переменные и несколько выражений
curl -i -X POST http://localhost:8080/calc -d "a=2+3; a*10"
curl -i -X POST http://localhost:8080/calc -d "a=10; b=a/2; b+1"
```

Ошибки (сервер возвращает `400 Bad Request`):

```bash
curl -i -X POST http://localhost:8080/calc -d "10/0"
curl -i -X POST http://localhost:8080/calc -d "x+1"   # неизвестная переменная
```

---

## Local CLI calculator (`calc_cli`)

`calc_cli` вычисляет выражение/программу локально (без HTTP).

```bash
./build/calc_cli "2*(3+4)"
./build/calc_cli "10/0"
./build/calc_cli "a=2+3; a*10"
./build/calc_cli "a=10; b=a/2; b+1"
```

---

## CLI HTTP client (`calc`)

`calc` — консольный клиент, который отправляет выражение на HTTP сервер `/calc`.
Перед использованием сервер должен быть запущен:

```bash
./build/echo_server
```

Примеры:

```bash
./build/calc -e 2 + 2
./build/calc -e 10/0
```

Если выражение содержит `;`, в обычном терминале лучше использовать кавычки:

```bash
./build/calc -e "a=2+3; a*10"
./build/calc -e "a=10; b=a/2; b+1"
```

---

## Build & Run (Docker)

Сборка образа:

```bash
docker build -t the_game_of_dev_cpp .
```

Запуск контейнера (порт 8080 проброшен на хост):

```bash
docker run --rm -it -p 8080:8080 -v "$PWD":/workspace -w /workspace the_game_of_dev_cpp bash
```

### Inside container

Сборка:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

Запуск сервера:

```bash
./build/echo_server
```

Тест с хоста:

```bash
curl -i -X POST http://localhost:8080/calc -d "2*(3+4)"
curl -i -X POST http://localhost:8080/calc -d "a=2+3; a*10"
```

---

## VS Code Dev Container

1) Установи расширение VS Code: **Dev Containers**  
2) Открой папку проекта в VS Code  
3) `F1 → Dev Containers: Reopen in Container`

Внутри контейнера:

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/echo_server
```

---

## Level 10: Server state (single shared Env) + clean

На уровне 10 сервер хранил переменные в одном общем `Env` (на весь сервер), а команда `clean`
очищала его полностью.

Начиная с Level 11 состояние стало **per-session** (см. ниже).

---

## Level 11 — Sessions (per-client state)

Сервер хранит состояние (переменные калькулятора) **по сессиям**.

### Как работает сессия

- Клиент `calc` отправляет заголовок `X-Session-Id` (если он уже есть).
- Если `X-Session-Id` не пришёл — сервер генерирует новый и возвращает его обратно в ответе.
- Клиент `calc` сохраняет session id локально в файле **`.calc_session`** в *текущей директории*.
  Поэтому разные папки = разные файлы `.calc_session` = разные сессии.
- Команда `clean` сбрасывает состояние **только текущей сессии**, не влияя на другие.

### CLI usage

Команды:
```bash
./build/calc -c echo
./build/calc -c clean
```

Вычисления:
```bash
./build/calc -e 2 + 2
./build/calc -e "a=2+3; a*10"
./build/calc -e 10/0
```

### Две независимые сессии (самый простой способ)

Самый простой способ создать 2 независимые сессии — работать из разных папок
(так у каждой будет свой `.calc_session`).

Terminal A:
```bash
mkdir -p sessA
cd sessA
../build/calc -e pi=3.14
../build/calc -e 2 \* pi \* 3
../build/calc -c clean
../build/calc -e 2 \* pi \* 3   # ошибка: pi очищен только в sessA
```

Terminal B:
```bash
mkdir -p sessB
cd sessB
../build/calc -e pi=1
../build/calc -e 2 \* pi \* 3   # работает и не зависит от sessA
```

### Сессии по юзеру

Если включён режим "по юзеру", клиент добавляет заголовок `X-User` (например через ключ `-u`),
и сервер хранит состояние отдельно для каждой пары `(user, session_id)`.

Пример:
```bash
./build/calc -u student -e pi=3.14
./build/calc -u student -e 2 \* pi \* 3
./build/calc -u admin   -e 2 \* pi \* 3   # будет ошибка, т.к. другой user
```

---

## Branches / Levels (history)

Разработка велась по уровням (веткам). Ветка [`main`](https://github.com/shishkarev/The_Game_Of_Dev/tree/main) содержит **финальный рабочий вариант**.

| Level | Branch | Что добавлено |
|------:|--------|---------------|
| 1–4   | [`level_1-4`](https://github.com/shishkarev/The_Game_Of_Dev/tree/level_1-4) | Базовый каркас проекта, сборка, hello world |
| 5     | [`level_5`](https://github.com/shishkarev/The_Game_Of_Dev/tree/level_5) | HTTP сервер `echo_server` + эндпоинт `/echo` |
| 6     | [`level_6`](https://github.com/shishkarev/The_Game_Of_Dev/tree/level_6) | Ядро калькулятора: лексер/парсер/AST/eval + `calc_cli` (локальный) |
| 7     | [`level_7`](https://github.com/shishkarev/The_Game_Of_Dev/tree/level_7) | Интеграция калькулятора в сервер: `/calc` |
| 8     | [`level_8`](https://github.com/shishkarev/The_Game_Of_Dev/tree/level_8) | CLI HTTP-клиент `calc` (`-e`, `-c`, поддержка `calc -e 2 + 2`) |
| 9     | [`level_9`](https://github.com/shishkarev/The_Game_Of_Dev/tree/level_9) | Мини-язык: переменные `a=...`, несколько выражений через `;` |
| 10    | [`level_10`](https://github.com/shishkarev/The_Game_Of_Dev/tree/level_10) | Состояние на сервере (переменные живут между запросами) + `clean` |
| 11    | [`level_11`](https://github.com/shishkarev/The_Game_Of_Dev/tree/level_11) | Сессии (per-client state) через `X-Session-Id`; `clean` чистит только текущую сессию |

**Final:** [`main`](https://github.com/shishkarev/The_Game_Of_Dev/tree/main) = состояние проекта после `level_11` (все фичи включены).
