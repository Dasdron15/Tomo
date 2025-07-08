#include "fileio.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"

void load_file(const char *path) {
    FILE *fp = fopen(path, "r");
    char buffer[1024];
    int i = 0;

    if (fp == NULL) {
        printf("File not found\n");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        editor.lines[i++] = strdup(buffer);
    }

    fclose(fp);
    editor.total_lines = i;
}

void save_file() {
    FILE *fp = fopen(editor.filename, "w");

    for (int i = 0; i < editor.total_lines && editor.lines[i] != NULL; i++) {
        if (i != editor.total_lines - 1) {
            fprintf(fp, "%s\n", editor.lines[i]);
        } else {
            fprintf(fp, "%s\n", editor.lines[i]);
        }
    }
    fclose(fp);
    editor.is_saved = true;
}
