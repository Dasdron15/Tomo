#include <curses.h>
#include "editor.h"

int main(int argc, char* argv[]) {
    Editor_State state;

    init_editor(&state);
    draw_editor(&state);

    while (1) {
        int key = getch();

        move_cursor(key, state.cursor_x, state.cursor_y, &state);
        handle_key(key, &state);
    }

    return 0;
}
