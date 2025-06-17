CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -lm
LDFLAGS = -lcurses

SRC = src/main.c src/editor.c src/select.c src/utils/fileio.c src/utils/common.c src/ui/status_bar.c
OBJECTS = $(SRC:.c=.o)
TARGET = cobalt

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	@rm -f $(TARGET) $(OBJECTS) core
