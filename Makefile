# Makefile для проекта vpd_lab2 (Windows, MinGW)

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -O0
TARGET = vpd_lab2.exe
OBJ = main.o device.o timer.o

# Первая цель в файле выполняется по команде make без аргументов.
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

main.o: main.c device.h timer.h
	$(CC) $(CFLAGS) -c main.c -o main.o

device.o: device.c device.h
	$(CC) $(CFLAGS) -c device.c -o device.o

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) -c timer.c -o timer.o

run: $(TARGET)
	./$(TARGET)

clean:
	-cmd /c del /Q $(OBJ) $(TARGET)

.PHONY: run clean