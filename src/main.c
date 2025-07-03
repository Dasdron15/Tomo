#include "editor.h"
#include "fileio.h"
#include <curses.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

struct Editor_State *state;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    state = malloc(sizeof(struct Editor_State));

    char resolved_path[PATH_MAX];
    state->filename = realpath(argv[1], resolved_path);

    load_file(state->filename, state);
    init_editor(state);

    while (1) {
        draw_editor(state);
        int key = getch();
        move_cursor(key, state, false);
        handle_key(key, state);
    }

    endwin();
    return 0;
}
