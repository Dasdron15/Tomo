#include "fileio.h"
#include "../editor.h"

void load_file(const char* path, struct Editor_State *state) {
    FILE* fp = fopen(path, "r");
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

        state->lines[i++] = strdup(buffer);
    }


    fclose(fp);
    state->total_lines = i;
}

void save_file(struct Editor_State *state) {
    FILE* fp = fopen(state->filename, "w");

    for (int i = 0; i < state->total_lines && state->lines[i] != NULL; i++) {
        if (i != state->total_lines - 1) {
            fprintf(fp, "%s\n", state->lines[i]);
        } else {
            fprintf(fp, "%s\n", state->lines[i]);
        }
    }
    fclose(fp);
    state->is_saved = true;
}
