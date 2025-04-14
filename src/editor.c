#include "editor.h"

void init_editor(Editor_State *state) {
    int line_number_width = (int)floor(log10(state->total_lines)) + 1;
    state->cursor_x = line_number_width + 4;
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

    move(state->cursor_y, state->cursor_x);
}

void update_lines(Editor_State *state) {
    int line = 0;
    int len = 0;
    int index = 0;

    for (int i = 0; i < state->text_len; i++) {
        if (state->text[i] == '\n') {
            state->lines_len[line++] = len;
            state->words_start[index++] = i;
            len = 0;
        } else {
            len++;
        }
    }

    if (len > 0 || line == 0) {
        state->lines_len[line++] = len;
    }

    state->total_lines = line + 1;
}

void draw_editor(Editor_State* state) {
    clear();

    int line_number_width = (int)floor(log10(abs(state->total_lines))) + 1;
    int text_start_x = line_number_width + 3;

    int first_visible_line = getmaxy(stdscr);
    int last_visible_line;

    int row = 0;
    int col = 0;

    for (int i = 0; i < state->text_len + 1; i++) {
        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(2));
        if (row == state->cursor_y) {
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
        }
        mvprintw(row, 0, "%s%d  ",mult_char(' ', line_number_width - (floor(log10(abs(row + 1))) + 1)) ,row + 1);
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));

        if (state->text[i] == '\n') {
            row++;
            state->lines_len[row] = col;
            col = 0;
        } else {
            mvprintw(row, text_start_x + col++, "%c", state->text[state->words_start[state->scroll_offset] + i]);
        }
    }

    debug_draw(state);
    move(state->cursor_y, state->cursor_x);
    refresh();
}


void move_cursor(int key, Editor_State *state) {
    int line_number_width = (int)floor(log10(abs(state->total_lines))) + 1;
    int text_start_x = line_number_width + 3;
    int cursor_x_in_line = state->cursor_x - text_start_x;
    int last_line = getmaxy(stdscr) > state->total_lines ? state->total_lines : getmaxy(stdscr);

    switch (key) {
        case KEY_UP:
            if (state->cursor_y > 0) {
                state->cursor_y--;
                int len = state->lines_len[state->cursor_y + 1];
                if (cursor_x_in_line > len) {
                    state->cursor_x = len + text_start_x;
                }
            }
            break;

        case KEY_DOWN:

            if (state->cursor_y < state->total_lines - 1) {
                if (state->cursor_y > last_line - 5) {
                    state->scroll_offset++;
                } else {
                    state->cursor_y++;
                    int len = state->lines_len[state->cursor_y + 1];
                    if (cursor_x_in_line > len) {
                        state->cursor_x = len + text_start_x;
                    }
                }
            }
            break;

        case KEY_LEFT:
            if (state->cursor_x > text_start_x) {
                state->cursor_x--;
            } else if (state->cursor_y > 0) {
                state->cursor_y--;
                int len = state->lines_len[state->cursor_y + 1];
                if (cursor_x_in_line > 0) {
                    state->cursor_x = len + text_start_x;
                } else {
                    state->cursor_x = text_start_x;
                }
            }
            break;

        case KEY_RIGHT: {
            int len = state->lines_len[state->cursor_y + 1];
            if (cursor_x_in_line < len) {
                state->cursor_x++;
            } else if (state->cursor_y < state->total_lines - 1) {
                state->cursor_y++;
                state->cursor_x = text_start_x;
            }
            break;
        }
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

void debug_draw(Editor_State *state) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    mvprintw(0, max_x - 25, "First visible line: %d", state->scroll_offset + 1);
    mvprintw(1, max_x - 25, "Last visible line: %d", max_y > state->total_lines ? state->total_lines : max_y);
}
