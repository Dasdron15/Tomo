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
    char buffer[1024];
    int i = 0;

    if (fp == NULL) {
        fprintf(stderr, "Error: File not found\n");
        exit(1);
    }

    bool indent_measured = false;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        editor.lines[i++] = strdup(buffer);
        if (!indent_measured) {
            init_indent(buffer, &indent_measured);
        }
    }

    if (!indent_measured) {
        editor.tab_indent = false;
        editor.indent_size = DEFAULT_INDENT_SIZE;
    }
    
    fclose(fp);
    editor.total_lines = i;
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
        exit(0);
    }

    for (int i = 0; i < editor.total_lines && editor.lines[i] != NULL; i++) {
        if (i != editor.total_lines - 1) {
            fprintf(fp, "%s\n", editor.lines[i]);
        } else {
            fprintf(fp, "%s\n", editor.lines[i]);
        }
    }
    fclose(fp);
    return true;
}
