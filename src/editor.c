#include "editor.h"
#include "utils/common.h"

void init_editor(Editor_State *state) {
    state->cursor_x = 0;
    state->cursor_y = 0;
    state->scroll_offset = 0;

    initscr();
    set_escdelay(0);
    raw();
    keypad(stdscr, true);
    noecho();
    curs_set(2);

    if (has_colors() == FALSE) {
        endwin();
        printf("Error: Terminal does not support colors");
        exit(1);
    }

    start_color();

    init_color(COLOR_BLUE, 339, 339, 339);
    init_pair(1, COLOR_WHITE, use_default_colors()); // Active line number color
    init_pair(2, COLOR_BLUE, use_default_colors()); // Unactive line number color
}

void draw_editor(Editor_State* state) {
    clear();
    for (int row = 0; row < state->total_lines; row++) {
        mvprintw(row, 0, "%s%d  ", mult_char(' ', int_len(state->total_lines) - int_len(row)), row + 1);

        mvprintw(row, 2 + int_len(state->total_lines), "%s", state->lines[row]);
    }

    move(state->cursor_y, state->cursor_x);
    refresh();
}

void move_cursor(int key, Editor_State *state) {
    switch (key) {
        case KEY_UP:
            if (state->cursor_y > 0) {
                state->cursor_y--;
            }
            break;
            
        case KEY_DOWN:
            state->cursor_y++;
            break;
            
        case KEY_LEFT:
            if (state->cursor_x > 0) {
                state->cursor_x--;
            } else if (state->cursor_x < 1 && state->cursor_y > 0) {
                state->cursor_y--;
            }
            break;
            
        case KEY_RIGHT:
            if (state->cursor_x < strlen(state->lines[state->cursor_y]) - 1) {
                state->cursor_x++;
            }
            break;
    }
}

void handle_key(int key, Editor_State *state) {
    switch (key) {
        case 27:
            endwin();
            exit(0);
            break;
    }
}
