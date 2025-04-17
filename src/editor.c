#include "editor.h"

void init_editor(Editor_State *state) {
    state->cursor_x = int_len(state->total_lines) + 2;
    state->cursor_y = 0;
    state->scroll_offset = 0;
    state->max_char = int_len(state->total_lines) + 2;

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
    for (int row = 0; row < state->total_lines && state->lines[row + state->scroll_offset] != NULL; row++) {
        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(2)); // Change color to gray

        if (row == state->cursor_y) { // If cursor is on the line then that line number will be white
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
        }
        mvprintw(row, 0, "%s%d  ", mult_char(' ', int_len(state->total_lines) - int_len(row + 1 + state->scroll_offset)), state->scroll_offset + row + 1); // Print line numbers

        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));

        mvprintw(row, 2 + int_len(state->total_lines), "%s", state->lines[row + state->scroll_offset]); // Print lines
    }

    debug_draw(state);
    move(state->cursor_y, state->cursor_x);
    refresh();
}

void move_cursor(int key, Editor_State *state) {
    int last_line = getmaxy(stdscr) > state->total_lines ? state->total_lines : getmaxy(stdscr) + state->scroll_offset;
    int line_len = strlen(state->lines[state->cursor_y + state->scroll_offset]);
    int margin = int_len(state->total_lines) + 2; // Line number length + 2 spaces

    switch (key) {
        case KEY_UP:
            if (state->cursor_y > 0) {
                if (state->scroll_offset + 1 > 1 && state->cursor_y <= 3) {
                    state->scroll_offset--;
                } else {
                    state->cursor_y--;
                }
                state->cursor_x = state->max_char;
            }
            break;
            
        case KEY_DOWN:
            if (state->cursor_y != state->total_lines - 1 - state->scroll_offset) {
                if (state->cursor_y > last_line - 5 - state->scroll_offset && last_line != state->total_lines) {
                    state->scroll_offset++;
                } else {
                    state->cursor_y++;
                }
                state->cursor_x = state->max_char;
            }
            break;
            
        case KEY_LEFT: {
            if (state->cursor_x > margin) {
                state->cursor_x--;
            } else if (state->cursor_x <= margin) {
                state->cursor_y--;
                state->cursor_x = margin + line_len;
            }
            state->max_char = state->cursor_x;
            break;
        }
            
        case KEY_RIGHT:
            if (state->cursor_x < margin + line_len) {
                state->cursor_x++;
            } else if (state->cursor_y != state->total_lines - 1 && state->cursor_y + state->scroll_offset < state->total_lines) {
                state->cursor_y++;
                state->cursor_x = margin;
            }
            state->max_char = state->cursor_x;
            break;
    }

    if (state->cursor_y >= getmaxy(stdscr) && state->cursor_y + state->scroll_offset >= state->total_lines) {
        state->cursor_y = getmaxy(stdscr) - 1;
    } else if (state->cursor_y >= getmaxy(stdscr)) {
        state->scroll_offset++;
        state->cursor_y--;
        state->cursor_x = margin;
    }

    if (state->cursor_x > margin + strlen(state->lines[state->cursor_y + state->scroll_offset])) { // Check if cursor is out of line
        state->cursor_x = margin + strlen(state->lines[state->cursor_y + state->scroll_offset]);
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

void debug_draw(Editor_State *state) {
    int y = getmaxy(stdscr) > state->total_lines ? state->total_lines : getmaxy(stdscr) + state->scroll_offset;

    mvprintw(0, getmaxx(stdscr) - 30, "First visible line: %d", state->scroll_offset + 1);
    mvprintw(1, getmaxx(stdscr) - 30, "Last visible line: %d", y);
}