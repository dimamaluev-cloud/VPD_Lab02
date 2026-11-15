# VPD Lab 02

Лабораторная работа №2 по языку программирования C.

## Состав проекта

- main.c — основная программа
- device.c, device.h — модуль устройства
- timer.c, timer.h — модуль измерения времени
- Makefile — сборка через GNU Make
- CMakeLists.txt — сборка через CMake

## Сборка через GCC

```bash
gcc -Wall -Wextra -Wpedantic -O0 -o vpd_lab2.exe main.c device.c timer.c
```

## Сборка через Make

```bash
make
```

Запуск:

```bash
make run
```

Очистка:

```bash
make clean
```

## Сборка через CMake

Создание конфигурации:

```bash
cmake -S . -B build -G "MinGW Makefiles"
```

Компиляция:

```bash
cmake --build build
```

Запуск:

```powershell
.\build\vpd_lab2.exe
```

## Игнорируемые файлы

Папка build и временные файлы компиляции исключены через .gitignore.