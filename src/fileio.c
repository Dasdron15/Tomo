#include "fileio.h"

void load_file(const char* path, Editor_State *state) {
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        perror("Error opening file");
        endwin();
        exit(0);
    }

    state->text_len = fread(state->text, 1, sizeof(state->text), fp);

    fclose(fp);
}

void save_file(const char* path, const Editor_State *state) {
    FILE *fp = fopen(path, "w");

    if (fp == NULL) {
        perror("Error writing to a file");
        return;
    }

    fprintf(fp, "%s", state->text);
    fclose(fp);
}
