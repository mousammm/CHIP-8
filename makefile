CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SDL_FLAGS = -lSDL2

SRC = src/main.c src/chip8/chip8.c src/sdl/sdl.c
OBJ = $(SRC:.c=.o)
TARGET = chip8

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(SDL_FLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean
