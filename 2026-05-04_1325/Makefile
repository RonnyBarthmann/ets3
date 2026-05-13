# Makefile für SDL2 2D Programm

# Compiler
CC = gcc

# Compiler-Flags
CFLAGS = -Wall -Wextra -O2 -g

# Linker-Flags (SDL2 wird über pkg-config eingebunden)
LDFLAGS = $(shell pkg-config --libs sdl2) -lm -ldl -fPIC -pie -mcmodel=large -Wl,--relax -Wl,--no-relax -fno-jump-tables -llua5.4
CFLAGS += $(shell pkg-config --cflags sdl2) -fPIC -pie -mcmodel=large -Wl,--relax -Wl,--no-relax -fno-jump-tables -I/usr/include/lua5.4

# Ziel-Executable
TARGET = ets3

# Quell-Dateien
SOURCES = main.c sdl.c

# Objekt-Dateien (automatisch aus SOURCES erzeugt)
OBJECTS = $(SOURCES:.c=.o)

# Standard-Ziel
all: $(TARGET)

# Linken des Executables
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Kompilieren der .c → .o Dateien
%.o: %.c sdl.h
	$(CC) $(CFLAGS) -c $< -o $@

# Aufräumen
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony-Targets (nicht mit Dateinamen kollidieren)
.PHONY: all clean

# Optional: Ausführen nach dem Bauen
run: $(TARGET)
	./$(TARGET)
