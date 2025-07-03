#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <curses.h>

#include "editor.h"
#include "fileio.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char resolved_path[PATH_MAX];
    editor.filename = realpath(argv[1], resolved_path);

    load_file(editor.filename);
    init_editor();

    while (1) {
        draw_editor();
        int key = getch();
        move_cursor(key, false);
        handle_key(key);
    }

    endwin();
    return 0;
}
