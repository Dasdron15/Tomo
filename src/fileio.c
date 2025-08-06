#include "fileio.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "editor.h"
#include "init.h"

void load_file(const char *path) {
    FILE *fp = fopen(path, "r");
    char buffer[1024];
    int i = 0;

    if (fp == NULL) {
        printf("File not found\n");
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

void save_file(void) {
    FILE *fp = fopen(editor.filename, "w");

    for (int i = 0; i < editor.total_lines && editor.lines[i] != NULL; i++) {
        if (i != editor.total_lines - 1) {
            fprintf(fp, "%s\n", editor.lines[i]);
        } else {
            fprintf(fp, "%s\n", editor.lines[i]);
        }
    }
    fclose(fp);
}
