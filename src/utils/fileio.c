#include "fileio.h"
#include "../editor.h"

void load_file(const char* path, struct Editor_State *state) {
    FILE* fp = fopen(path, "r");
    char buffer[1024];
    int i = 0;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        state->lines[i++] = strdup(buffer);
    }

    fseek(fp, -1, SEEK_END);
    char last;
    fread(&last, 1, 1, fp);
    if (last == '\n') {
        state->lines[i++] = strdup("\n");
    }

    fclose(fp);
    state->total_lines = i;
}

void save_file(struct Editor_State *state) {
    FILE* fp = fopen(state->filename, "w");

    for (int i = 0; i < state->total_lines && state->lines[i] != NULL; i++) {
        fprintf(fp, "%s", state->lines[i]);
    }
    fclose(fp);
    state->is_saved = true;
}
