#include "fileio.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <dirent.h>

#include "editor.h"
#include "file_tree.h"
#include "init.h"

void load_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        endwin();
        reset();
        fprintf(stderr, "Error: File cannot be opened\n");
        exit(1);
    }

    bool indent_measured = false;

    size_t line_capacity = 128;
    int line_count = 0;
    editor.lines = malloc(sizeof(char*) * line_capacity);
    if (!editor.lines) {
        fclose(fp);
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(1);
    }

    size_t capacity = 128;
    size_t length = 0;
    char *line = malloc(capacity);
    if (!line) {
        fclose(fp);
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(1);
    }

    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (line_count >= line_capacity) {
            line_capacity *= 2;
            editor.lines = realloc(editor.lines, sizeof(char*) * line_capacity);
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

    // Handle empty last line
    if (ch == EOF && length == 0) {
        editor.lines[line_count++] = strdup("");
    }

    free(line);
    fclose(fp);

    if (!indent_measured) {
        editor.tab_indent = false;
        editor.indent_size = DEFAULT_INDENT_SIZE;
    }

    editor.capacity = line_capacity;
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
        fprintf(stderr, "Error: file was not found\n");
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

void open_dir(char *path) {
    int capacity = 4;
    char **elements = malloc(sizeof(char*) * capacity);
    if (!elements) {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        exit(1);
    }

    struct dirent *de;
    DIR *dr = opendir(path);
    if (dr == NULL) {
        fprintf(stderr, "Cannot open a directory: %s\n", path);
        exit(1);
    }

    int i = 0;
    while ((de = readdir(dr)) != NULL) {
        if (i >= capacity) {
            capacity *= 2;
            elements = realloc(elements, sizeof(char*) * capacity);
        }

        elements[i] = strdup(de->d_name);
        i++;
    }

    draw_tree(elements, i, path);

    for (int k = 0; k < i; k++) {
        free(elements[k]);
    }

    free(elements);
    closedir(dr);
}
