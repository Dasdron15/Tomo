CC = gcc
CFLAGS = -Wall -O2 -Wextra -g -lm
LDFLAGS = -lcurses

SRC = src/main.c src/editor.c src/utils/fileio.c src/utils/common.c src/ui/status_bar.c
OBJECTS = $(SRC:.c=.o)
TARGET = cobalt

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	@rm -f $(TARGET) $(OBJECTS) core
