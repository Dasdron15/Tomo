#include "fileio.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>

#include "editor.h"
#include "init.h"

void load_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Error: Can't open file");
        exit(1);
    }


    bool indent_measured = false;

    size_t line_capacity = 128;
    int line_count = 0;
    editor.lines = malloc(line_capacity);

    size_t capacity = 128;
    size_t length = 0;
    char *line = malloc(capacity);

    if (!line) {
        fclose(fp);
        fprintf(stderr, "Error: memory not allocated");
        exit(1);
    }

    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (line_count >= line_capacity) {
            line_capacity *= 2;
            editor.lines = realloc(editor.lines, line_capacity);
        }

        if (length >= capacity) {
            capacity *= 2;
            line = realloc(line, capacity);
        }

        if (ch == '\n') {
            line[length] = '\0';

            if (!indent_measured) {
                init_indent(line, &indent_measured);
            }

            editor.lines[line_count++] = strdup(line);
            length = 0;
            continue;
        }

        line[length++] = (char)ch;
    }

    if (length > 0) {
        line[length] = '\0';
        editor.lines[line_count++] = strdup(line);
    }

    if (ch == EOF && length == 0) {
        editor.lines[line_count++] = strdup("");
    }

    free(line);
    fclose(fp);

    if (!indent_measured) {
        editor.tab_indent = false;
        editor.indent_size = DEFAULT_INDENT_SIZE;
    }

    for (int i = 0; i < line_count; i++) {
        printf("%s", editor.lines[i]);
    }

    exit(0);

    editor.total_lines = line_count;
}

bool save_file(void) {
    if (access(editor.filename, W_OK) != 0) {
        editor.bottom_text = "Cannot save: file is read-only";
        return false;
    }

    FILE *fp = fopen(editor.filename, "w");
    if (!fp) {
        endwin();
        reset();
        fprintf(stderr, "Error: file was not found");
        exit(1);
    }

    for (int i = 0; i < editor.total_lines && editor.lines[i]; i++) {
        if (i != editor.total_lines - 1) {
            fprintf(fp, "%s\n", editor.lines[i]);
        } else {
            fprintf(fp, "%s", editor.lines[i]);
        }
    }
    fclose(fp);
    return true;
}
