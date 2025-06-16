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

void save_pos(struct Editor_State *state) {
    FILE* src = fopen("src/cursor_pos.txt", "r");
    FILE* tmp = fopen("src/cursor_pos_temp.txt", "w");

    if (!src) {
        timeout(10000);
    }

    char line[1024];
    int found = 0;
    size_t name_len = strlen(state->filename);

    if (src) {
        while (fgets(line, sizeof(line), src)) {
            if (!found && strstr(line, state->filename) != NULL && line[name_len] == ':') {
                fprintf(tmp, "%s:%d:%d:%d", state->filename, state->cursor_y, state->cursor_x, state->scroll_offset);
                found = 1;
            }
            else {
                fputs(line, tmp);
            }
        }
        fclose(src);
    }

    if (!found) {
        fprintf(tmp, "%s:%d:%d:%d", state->filename, state->cursor_y, state->cursor_x, state->scroll_offset);
    }

    fclose(tmp);

    rename("src/cursor_pos.txt", "src/backup.txt");
    rename("src/cursor_pos_temp.txt", "src/cursor_pos.txt");
    rename("src/backup.txt", "src/cursor_pos_temp.txt");
}

int load_pos(struct Editor_State *state) {
    FILE *src = fopen("src/cursor_pos.txt", "r");

    char line[1024];
    int loaded = 0;

    while (fgets(line, sizeof(line), src)) {
        char *token = strtok(line, ":");
        if (!token) continue;

        if (strcmp(token, state->filename) != 0) {
            continue;
        }

        int count = 1;
        int values[3] = {0};

        while ((token = strtok(NULL, ":\n")) && count <= 3) {
            values[count-1] = atoi(token);
            count++;
        }

        if (count == 4) {
            state->cursor_y = values[0];
            state->cursor_x = values[1];
            state->scroll_offset = values[2];
            loaded = 1;
            break;
        }
    }

    fclose(src);
    return loaded;
}
