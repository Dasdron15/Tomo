CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lcurses

SOURCES = src/main.c src/editor.c src/utils/fileio.c src/utils/common.c src/ui/status_bar.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = cobalt

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	@rm -f $(TARGET) $(OBJECTS) core
