#include <curses.h>
#include "editor.h"
#include "fileio.h"

int main(int argc, char* argv[]) {
    Editor_State state;
    Text_Buffer buffer;

    
    init_editor(&state);
    load_file(argv[1], &buffer);
    draw_editor(&state);

    while (1) {
        int key = getch();

        move_cursor(key, state.cursor_x, state.cursor_y, &state);
        handle_key(key, &state);
    }

    return 0;
}
