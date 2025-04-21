#include "editor.h"
#include "ui/status_bar.h"
#include "utils/fileio.h"
#include "utils/common.h"

void init_editor(struct Editor_State *state) {
    state->cursor_x = int_len(state->total_lines) + 2;
    state->max_char = state->cursor_x;
    state->cursor_y = 0;
    state->scroll_offset = 0;
    state->is_saved = true;

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
    init_color(COLOR_CYAN, 187, 187, 187);

    init_pair(1, COLOR_WHITE, use_default_colors()); /* Active line number color */
    init_pair(2, COLOR_BLUE, use_default_colors()); /* Unactive line number color */
    init_pair(3, COLOR_WHITE, COLOR_CYAN); /* Color for status bar */
}

void draw_editor(struct Editor_State* state) {
    int margin = int_len(state->total_lines) + 2;

    clear();
    for (int row = 0; row < state->total_lines && state->lines[row + state->scroll_offset] != NULL; row++) {
        char* spaces = mult_char(' ', int_len(state->total_lines) - int_len(row + 1 + state->scroll_offset));

        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(2)); /* Change color to gray */

        if (row == state->cursor_y) { /* If cursor is on the line then that line number will be white */
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
        }
        mvprintw(row, 0, "%s%d  ", spaces, state->scroll_offset + row + 1); /* Print line numbers */

        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));

        mvprintw(row, 2 + int_len(state->total_lines), "%s", state->lines[row + state->scroll_offset]); /* Print lines */

        free(spaces);
        spaces = NULL;
    }

    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));

    draw_status_bar(state);

    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(1));

    move(state->cursor_y, state->cursor_x);
    refresh();
}

void move_cursor(int key, struct Editor_State* state) {
    int last_line = getmaxy(stdscr) > state->total_lines ? state->total_lines : getmaxy(stdscr) + state->scroll_offset;
    int line_len = strlen(state->lines[state->cursor_y + state->scroll_offset]);
    int margin = int_len(state->total_lines) + 2; /* Line number length + 2 spaces */

    switch (key) {
        case KEY_UP:
            if (state->cursor_y + state->scroll_offset > 0) {
                state->cursor_y--;
                state->cursor_x = state->max_char;
            } else {
                state->cursor_x = margin;
            }
            break;
            
        case KEY_DOWN:
            if (state->cursor_y != state->total_lines - 1 - state->scroll_offset) {
                state->cursor_y++;
                state->cursor_x = state->max_char;
            } else {
                state->cursor_x = margin + line_len;
            }
            break;
            
        case KEY_LEFT: {
            if (state->cursor_x > margin) {
                state->cursor_x--;
            } else if (state->cursor_x <= margin && state->cursor_y + state->scroll_offset >= 1) {
                state->cursor_y--;
                state->cursor_x = margin + strlen(state->lines[state->cursor_y + state->scroll_offset]);
            }
            state->max_char = state->cursor_x;
            break;
        }
            
        case KEY_RIGHT:
            if (state->cursor_x < margin + line_len) {
                state->cursor_x++;
            } else if (state->cursor_y != state->total_lines - 1 && state->cursor_y + state->scroll_offset + 1 < state->total_lines) {
                state->cursor_y++;
                state->cursor_x = margin;
            }
            state->max_char = state->cursor_x;
            break;
    }

    clamp_cursor(state);

    if (state->cursor_x > margin + strlen(state->lines[state->cursor_y + state->scroll_offset])) { /* Check if cursor is out of line */
        state->cursor_x = margin + strlen(state->lines[state->cursor_y + state->scroll_offset]);
    }
}

void clamp_cursor(struct Editor_State* state) { /* Check if cursor is out of editor window */
    if (state->cursor_y >= getmaxy(stdscr) - 1) {
        state->scroll_offset++;
        state->cursor_y--;
    } else if (state->cursor_y < 0 && state->scroll_offset > 0) {
        state->scroll_offset--;
        state->cursor_y++;
    }
}

void handle_key(int key, struct Editor_State* state) {
    if (key == 27) {
        endwin();
        exit(0);
        return;
    }

    if ((key >= 32 && key <= 126)) {
        insert_char((char)key, state);
        state->max_char = state->cursor_x;
        state->is_saved = false;
        return;
    }

    if (key == '\t') {
        add_tab(state);
        state->max_char = state->cursor_x;
        state->is_saved = false;
        return;
    }

    if (key == KEY_BACKSPACE || key == 127) {
        delete_char(state);
        state->max_char = state->cursor_x;
        state->is_saved = false;
        return;
    }

    if (key == KEY_ENTER || key == 10) {
        new_line(state);
        state->is_saved = false;
        return;
    }

    if (key == 15) {
        save_file(state);
        return;
    }
}

void insert_char(char c, struct Editor_State* state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x - margin;

    char* old = state->lines[state->cursor_y + state->scroll_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
    }

    char* new = malloc(strlen(old) + 2);

    if (new == NULL) {
        return;
    }

    strncpy(new, old, pos);
    new[pos] = c;
    strcpy(new + pos + 1, old + pos);

    state->lines[state->cursor_y + state->scroll_offset] = new;

    state->cursor_x++;
}

void add_tab(struct Editor_State* state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x - margin;

    char* old = state->lines[state->cursor_y + state->scroll_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
    }

    char* new = malloc(strlen(old) + 5);

    if (new == NULL) {
        return;
    }

    strncpy(new, old, pos);
    memcpy(new + pos, "    ", 4);
    strcpy(new + pos + 4, old + pos);

    state->lines[state->cursor_y + state->scroll_offset] = new;
    state->cursor_x += 4;
}

void delete_char(struct Editor_State* state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x - margin;
    int y_pos = state->scroll_offset + state->cursor_y;

    int len = strlen(state->lines[state->cursor_y]);

    if (pos > 0) {
        for (int i = pos - 1; i < len; i++) {
            state->lines[y_pos][i] = state->lines[y_pos][i + 1];
        }
        state->cursor_x--;
    } else {
        if (y_pos >= 1) {
            int size = strlen(state->lines[y_pos - 1]) + strlen(state->lines[y_pos]) + 1;
            char* buf = (char*)malloc(size);
            int previous_len = strlen(state->lines[y_pos - 1]) + margin;

            if (!buf) {
                return;
            }

            strcpy(buf, state->lines[y_pos - 1]);
            strcat(buf, state->lines[y_pos]);
            
            state->lines[y_pos - 1] = buf;

            free(state->lines[y_pos]);
            state->lines[y_pos] = NULL;

            for (int i = state->cursor_y; i < state->total_lines - 1; i++) {
                state->lines[i] = state->lines[i + 1];
            }

            state->total_lines--;
            state->cursor_x = previous_len;
            state->cursor_y--;
        }
    }
}

void new_line(struct Editor_State* state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x - margin;
    int y_pos = state->cursor_y + state->scroll_offset;
    
    char* current = state->lines[y_pos];
    int current_len = strlen(current);

    char* left = malloc(pos + 1);
    char* right = malloc(current_len - pos + 1);

    if (!left || !right) {
        return;
    }

    strncpy(left, current, pos);
    left[pos] = '\0';

    strcpy(right, current + pos);

    free(state->lines[y_pos]);

    for (int i = state->total_lines; i > y_pos; i--) {
        state->lines[i] = state->lines[i - 1];
    }

    state->lines[y_pos] = left;
    state->lines[y_pos + 1] = right;

    state->cursor_y++;
    clamp_cursor(state);
    state->cursor_x = margin;
    state->total_lines++;
}

void debug_draw(struct Editor_State *state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x - margin;

    mvprintw(0, getmaxx(stdscr) - 10, "%d", pos);
}
