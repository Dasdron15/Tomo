#include "fileio.h"

int load_file(const char* path, Editor_State *state) {
    char buffer[1024];
    int i = 0;
    FILE *fp = fopen(path, "r");

    if (!fp) {
        endwin();
        printf("Error cannot open file\n");
        exit(1);
    }

    while (fgets(buffer, 1024, fp)) {
        state->lines[i++] = strdup(buffer);
    }
    fclose(fp);
    return i;
}

void save_file(char *path, Editor_State *state) {

}
