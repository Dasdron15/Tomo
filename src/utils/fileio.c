#include "fileio.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"

#define DEFAULT_INDENT_SIZE 4

void load_file(const char *path) {
    FILE *fp = fopen(path, "r");
    char buffer[1024];
    int i = 0;

    if (fp == NULL) {
        printf("File not found\n");
        exit(1);
    }

    bool indent_measured = false;
    int indent_count = 0;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        if (buffer[0] == ' ' && buffer[1] == ' ' && !indent_measured) {
            indent_count = 0;
            for (int i = 0; i < (int) strlen(buffer); i++) {
                if (buffer[i] == ' ') {
                    indent_count++;
                } else {
                    indent_measured = true;
                    break;
                }
            }
        }

        editor.lines[i++] = strdup(buffer);
    }

    if (!indent_measured) {
        editor.indent_size = DEFAULT_INDENT_SIZE;
    } else {
        editor.indent_size = indent_count;
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
