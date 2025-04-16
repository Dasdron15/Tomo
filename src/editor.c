#include "editor.h"

void init_editor(Editor_State *state) {
    state->cursor_x = 4;
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
        attroff(COLOR_PAIR(1)); // Change color to gray
        attron(COLOR_PAIR(2));

        if (row == state->cursor_y) { // If cursor is on the line then that line number will be white
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
        }
        mvprintw(row, 0, "%s%d  ", mult_char(' ', int_len(state->total_lines) - int_len(row + 1)), row + 1);

        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));

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
            if (state->cursor_y != state->total_lines - 1) {
                state->cursor_y++;
            }
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
            } else if (state->cursor_y != state->total_lines - 1) {
                state->cursor_y++;
                state->cursor_x = 0;
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
