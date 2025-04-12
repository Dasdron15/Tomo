#include "editor.h"

void init_editor(Editor_State *state) {
    state->cursor_x = 0;
    state->cursor_y = 0;
    state->num_lines = 0;

    initscr();
    set_escdelay(25);
    raw();
    keypad(stdscr, true);
    noecho();
    curs_set(2);

    move(state->cursor_y, state->cursor_x);
}

void draw_editor(Editor_State *state) {
    clear();

    for (int i = 0; i < state->num_lines; i++) {
        mvprintw(i, 0, "%s", state->lines[i]);
    }

    refresh();

}

void move_cursor(int key, int x, int y, Editor_State *state) {
    switch (key) {
        case KEY_UP:
            if (y > 0) state->cursor_y--;
            break;

        case KEY_DOWN:
            if (y < LINES - 1) state->cursor_y++;
            break;

        case KEY_LEFT:
            if (x > 0) {
                state->cursor_x--;
            } else {
                state->cursor_y--;
            }
            break;

        case KEY_RIGHT:
            if (x < COLS - 1) state->cursor_x++;
            break;
    }
    move(state->cursor_y, state->cursor_x);
}

void handle_key(int key, Editor_State *state) {
    switch (key) {
        case 27:
            endwin();
            exit(0);
            break;
    }
}
