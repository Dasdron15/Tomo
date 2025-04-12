#include "fileio.h"

void load_file(const char *path, Editor_State *state) {
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    size_t len = 0;
    while (fgets(state->text, sizeof(state->text), fp)) {
        len += strlen(state->text) + len;
    }

    fclose(fp);
}

void save_file(const char *path, const Editor_State *state) {
    FILE *fp = fopen(path, "w");

    if (fp == NULL) {
        perror("Error writing to a file");
        return;
    }

    fprintf(fp, "%s", state->text);
    fclose(fp);
}
