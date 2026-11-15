# Makefile для проекта vpd_lab2 (Windows, MinGW)
# Версия с автоматическими переменными GNU Make: $@, $<, $^

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -O0
TARGET = vpd_lab2.exe
OBJ = main.o device.o timer.o

# Правило линковки.
#   $@ - имя цели            (здесь vpd_lab2.exe)
#   $^ - все зависимости     (здесь main.o device.o timer.o)
$(TARGET): $(OBJ)
	$(CC) $^ -o $@

# Одно шаблонное правило для всех .o-файлов.
#   %.o: %.c - любой файл .o получается из одноимённого .c
#   $< - первая зависимость  (здесь соответствующий .c-файл)
#   $@ - имя цели            (здесь соответствующий .o-файл)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Зависимости от заголовков (команды не нужны: их даёт шаблонное правило).
main.o: device.h timer.h
device.o: device.h
timer.o: timer.h

run: $(TARGET)
	./$(TARGET)

clean:
	-cmd /c del /Q $(OBJ) $(TARGET)

.PHONY: run clean
