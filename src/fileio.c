#include "fileio.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "editor.h"
#include "file_tree.h"
#include "init.h"

void load_file(const char *path) {
    FILE *fp = fopen(path, "r");
    
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

            editor.lines[line_count] = strdup(line);

            line_count++;
            length = 0;
            continue;
        }

        line[length++] = (char)ch;
    }

    if (length > 0) {
        line[length] = '\0';
        editor.lines[line_count] = strdup(line);
        line_count++;
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
    struct dirent *de;
    DIR *dr = opendir(path);
    if (dr == NULL) {
        fprintf(stderr, "Cannot open a directory: %s\n", path);
        exit(1);
    }

    int dir_capacity = 4;
    int file_capacity = 4;
    int dir_count = 0;
    int file_count = 0;

    char **directories = malloc(sizeof(char*) * dir_capacity);
    char **files = malloc(sizeof(char*) * file_capacity);

    if (!directories || !files) {
        fprintf(stderr, "Error: Memory not allocated");
        exit(1);
    }

    while ((de = readdir(dr)) != NULL) {
        if (dir_count >= dir_capacity) {
            dir_capacity *= 2;
            directories = realloc(directories, sizeof(char*) * dir_capacity);
        }

        if (file_count >= file_capacity) {
            file_capacity *= 2;
            files = realloc(files, sizeof(char*) * file_capacity);
        }

        // Check if it's a directory or a file
        if (de->d_type == DT_DIR) {
            directories[dir_count] = strdup(de->d_name);
            dir_count++;
        } else {
            files[file_count] = strdup(de->d_name);
            file_count++;
        }
    }

    char **elements = malloc(sizeof(char*) * (dir_count + file_count));
    
    // First put directories into elements array
    int element_count = 0;
    for (int i = 0; i < dir_count; i++) {
        elements[element_count++] = directories[i];
    }

    // Then put files
    for (int i = 0; i < file_count; i++) {
        elements[element_count++] = files[i];
    }

    draw_tree(elements, dir_count + file_count, path);

    free(directories);
    free(files);
    for (int i = 0; i < (dir_count + file_count); i++) {
        free(elements[i]);
    }

    free(elements);

    closedir(dr);
}