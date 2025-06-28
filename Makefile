CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -lm
LDFLAGS = -lcurses

SRC = src/main.c src/editor.c src/select.c src/utils/fileio.c src/utils/common.c src/ui/status_bar.c
OBJECTS = $(SRC:.c=.o)
TARGET = cobalt
INSTALL_DIR = /usr/local/bin

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean install uninstall

clean:
	@rm -f $(TARGET) $(OBJECTS) core

install: $(TARGET)
	@cp $(TARGET) $(INSTALL_DIR)/$(TARGET)
	@chmod 755 $(INSTALL_DIR)/$(TARGET)
	@echo "$(TARGET) installed to $(INSTALL_DIR)"

uninstall:
	@rm -f $(INSTALL_DIR)/$(TARGET)
	@echo "$(TARGET) uninstalled from $(INSTALL_DIR)"
