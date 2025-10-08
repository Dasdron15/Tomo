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

    if (S_ISDIR(dir_path.st_mode)) {
        init_curses();
        init_colors();

        open_dir(argv[1]);
    } else {
        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            fp = fopen(argv[1], "w");
        }

        fclose(fp);

        char resolved_path[PATH_MAX];
        editor.filename = realpath(argv[1], resolved_path);

        init_curses();
        init_colors();
        load_file(editor.filename);
    }

    init_variables();
    syntax_init();
    init_undo_redo_stacks();

    curs_set(1);


    
    while (1) {
        syntax_reparse();
        draw_editor();
        int key = getch();
        handle_key(key);
    }

    endwin();
    destroy_undo_redo_stacks();
    return 0;
}
