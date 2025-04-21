#include "fileio.h"
#include "../editor.h"

void load_file(const char* path, struct Editor_State *state) {
    FILE* fp = fopen(path, "r");
    char buffer[1024];
    int i = 0;
    bool was_last_newline = false;

    while (fgets(buffer, sizeof(buffer), fp)) {
        state->lines[i++] = strdup(buffer);
        size_t len = strlen(buffer);
        was_last_newline = len > 0 && buffer[len - 1] == '\n';
    }

    if (was_last_newline) {
        int ch = getc(fp);
        if (ch == EOF) {
            state->lines[i++] = strdup("\n");
        } else {
            ungetc(ch, fp);
        }
    }
    fclose(fp);

    state->total_lines = i;
}

void save_file(struct Editor_State *state) {
    FILE* fp = fopen(state->filename, "w");

    for (int i = 0; i < state->total_lines && state->lines[i] != NULL; i++) {
        fprintf(fp, "%s\n", state->lines[i]);
    }
    fclose(fp);
    state->is_saved = true;
}
