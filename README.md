# The_Game_Of_Dev

## Build & Run (local)

```bash
mkdir -p build
cmake -S . -B build
cmake --build build
./build/The_Game_Of_Dev
```

## Build & Run (Docker)

```bash
docker build -t the_game_of_dev_cpp .
docker run --rm -it -v "$PWD":/workspace -w /workspace the_game_of_dev_cpp bash
```

## Inside container

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/The_Game_Of_Dev
```

## VS Code Dev Container
Install VS Code extension: Dev Containers
Open this folder in VS Code
F1 → Dev Containers: Reopen in Container