# TODO: Fix this shit

CC = gcc
CFLAGS = -lcurses
SRC = src/main.c src/editor.c src/utils/common.c src/utils/fileio.c
OBJ = $(SRC:.c=.o)
TARGET = cobalt.out

$(TARGET): $(OBJ):
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS)

src/main.o: src/main.c
	$(CC) -c src/main.c -o src/main.o

src/editor.o: src/editor.c
	$(CC) -c src/editor.c -o src/editor.o

src/utils/fileio.o: src/utils/fileio.c
	$(CC) -c src/utils/fileio.c -o src/utils/fileio.o

src/utils/common.o: src/utils/common.c
	$(CC) -c src/utils/common.c -o src/utils/common.o

clean:
	rm -f $(OBJ) $(TARGET)