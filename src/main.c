#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <curses.h>

#include "editor.h"
#include "fileio.h"
#include "init.h"
#include "syntax.h"
#include "themes.h"
#include "undo.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    struct stat dir_path;
    stat(argv[1], &dir_path);

    init_editor();
    init_colors();
    syntax_init();
    init_undo_stack();

    if (S_ISDIR(dir_path.st_mode)) {
        open_dir(argv[1]);
    } else {
        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            fp = fopen(argv[1], "w");
        }

        char resolved_path[PATH_MAX];
        editor.filename = realpath(argv[1], resolved_path);
        load_file(editor.filename);
    }



    while (1) {
        syntax_reparse();
        draw_editor();
        int key = getch();
        handle_key(key);
    }

    endwin();
    return 0;
}
