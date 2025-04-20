#include "fileio.h"
#include "../editor.h"

int load_file(const char* path, struct Editor_State *state) {
    char buffer[1024];
    int i = 0;
    FILE *fp = fopen(path, "r");

    if (!fp) {
        printf("Error: cannot open file\n");
        exit(1);
    }

    while (fgets(buffer, 1024, fp)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        state->lines[i++] = strdup(buffer);
    }
    fclose(fp);
    state->total_lines = i;
    return i;
}

void save_file(struct Editor_State *state) {
    FILE* fp = fopen(state->filename, "w");

    for (int i = 0; i < sizeof(state->lines) && state->lines[i] != NULL; i++) {
        fprintf(fp, "%s\n", state->lines[i]);
    }
    fclose(fp);
    state->is_saved = true;
}
