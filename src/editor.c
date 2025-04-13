#include "editor.h"

void init_editor(Editor_State *state) {
    state->cursor_x = 0;
    state->cursor_y = 0;

    initscr();
    set_escdelay(25);
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
    init_pair(1, COLOR_WHITE, use_default_colors()); // Active line color
    init_pair(2, COLOR_BLUE, use_default_colors()); // Unactive line color

    move(state->cursor_y, state->cursor_x);
}

void update_lines(Editor_State *state) {
    int line = 0, len = 0;

    for (int i = 0; i < state->text_len; i++) {
        if (state->text[i] == '\n') {
            state->lines_len[line++] = len;
            len = 0;
        } else {
            len++;
        }
    }

    if (len > 0 || line == 0) {
        state->lines_len[line++] = len;
    }

    state->total_lines = line;
}

void draw_editor(Editor_State* state) {
    clear();

    int row = 0, col = 0;
    for (int i = 0; i < state->text_len; i++) {
        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(2));
        if (row == state->cursor_y) {
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
        }
        mvprintw(row, 0, "%d  ", row + 1);
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));

        if (state->text[i] == '\n') {
            state->lines_len[row] = col;
            row++;
            col = 0;
        } else {
            mvprintw(row, 3 + col++, "%c", state->text[i]);
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
