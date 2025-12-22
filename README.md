# The_Game_Of_Dev

Проект для тестового задания.

Сборка даёт несколько бинарников:
- `The_Game_Of_Dev` — hello world
- `echo_server` — HTTP сервер (`/echo`, `/calc`)
- `calc_cli` — CLI калькулятор

---

## Build & Run (local)
```bash
mkdir -p build
cmake -S . -B build
cmake --build build
```

Запуск hello:
```bash
./build/The_Game_Of_Dev
```

Запуск HTTP сервера:
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
```

Запуск CLI калькулятора:
```bash
./build/calc_cli "2*(3+4)"
./build/calc_cli "10/0"
```

## Build & Run (Docker)

Сборка образа:
```bash
docker build -t the_game_of_dev_cpp .
```

Запуск контейнера (порт 8080 проброшен на хост):
```bash
docker run --rm -it -p 8080:8080 -v "$PWD":/workspace -w /workspace the_game_of_dev_cpp bash
```

## Inside container

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
```

## VS Code Dev Container

Установи расширение VS Code: Dev Containers
Открой папку проекта в VS Code
F1 → Dev Containers: Reopen in Container

Внутри контейнера можно собирать как обычно:
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

Запуск:
```bash
./build/echo_server
```