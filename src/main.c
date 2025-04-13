#include <curses.h>
#include "editor.h"
#include "fileio.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    Editor_State state;
    init_editor(&state);
    load_file(argv[1], &state);
    
    while (1) {
        draw_editor(&state);
        int key = getch();
        move_cursor(key, &state);
        handle_key(key, &state);
    }

    endwin();
    return 0;
}
