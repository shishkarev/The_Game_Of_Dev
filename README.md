# The_Game_Of_Dev

Проект для тестового задания.

Сборка даёт несколько бинарников:
- `The_Game_Of_Dev` — hello world
- `echo_server` — HTTP сервер (`/echo`, `/calc`)
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

Тест калькулятора (HTTP):

```bash
curl -i -X POST http://localhost:8080/calc -d "2*(3+4)"
curl -i -X POST http://localhost:8080/calc -d "10/0"
curl -i -X POST http://localhost:8080/calc -d "a=2+3; a*10"
curl -i -X POST http://localhost:8080/calc -d "a=10; b=a/2; b+1"
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
