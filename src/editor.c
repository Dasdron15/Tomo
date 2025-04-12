#include "editor.h"

void init_editor(Editor_State *state) {
    state->cursor_x = 0;
    state->cursor_y = 0;
    
    memset(state->lines_len, 1, sizeof(state->lines_len));

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

    int x = 0, y = 0;

    for (int i = 0; i < state->text_len; i++) {
        if (state->text[i] == '\n') {
            y++;
            x = 0;
        } else {
            mvprintw(y, x, "%c", state->text[i]);
            x++;
        }
    }

    move(state->cursor_y, state->cursor_x);
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
            } else if (x >= 0 && y > 0) {
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
