#include "editor.h"
#include "config.h"
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
    erase();
    int screen_width = getmaxx(stdscr);
    int screen_height = getmaxy(stdscr);
    
    unsigned int col = 0;
    unsigned int row = 0;
    
    for (int index = state->scroll_offset; index < screen_height - 1 && state->lines[index] != NULL; index++) {
        char* line = state->lines[index];
        unsigned int line_len = strlen(line);    

        for (int symb = 0; symb < line_len; symb++) {
            mvprintw(row, col++, "%c", line[symb]);
        }
        row++;
        col = 0;
    }

    attron(COLOR_PAIR(3));
    draw_status_bar(state);
    attroff(COLOR_PAIR(3));

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
                if (state->cursor_y < 1 && state->scroll_offset > 0) {
                    state->scroll_offset--;
                } else {
                    state->cursor_y--;
                    state->cursor_x = state->max_char;
                }
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

    if (state->cursor_x > margin + strlen(state->lines[state->cursor_y + state->scroll_offset])) { /* Check if cursor is out of line */
        state->cursor_x = margin + strlen(state->lines[state->cursor_y + state->scroll_offset]);
    }

    clamp_cursor(state);
}

void clamp_cursor(struct Editor_State* state) { /* Check if cursor is out of editor window */
    if (state->cursor_y >= getmaxy(stdscr) - 1) {
        state->cursor_y--;
        state->scroll_offset++;
    }
}

void handle_key(int key, struct Editor_State* state) {
    if (key == 17) {
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

    if (key == 19) {
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
        state->lines[state->cursor_y + state->scroll_offset] = old;
    }

    char* new = malloc(strlen(old) + 2);
    memset(new, 0, strlen(old) + 2);

    if (new == NULL) {
        return;
    }

    strncpy(new, old, pos);
    new[pos] = c;
    strcpy(new + pos + 1, old + pos);

    state->lines[state->cursor_y + state->scroll_offset] = new;

    free(old);
    state->cursor_x++;
}

void add_tab(struct Editor_State* state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x - margin;

    char* old = state->lines[state->cursor_y + state->scroll_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
        state->lines[state->cursor_y + state->scroll_offset] = old;
    }

    char* tab_str = mult_char(' ', TAB_SIZE);

    if (!tab_str) {
        return;
    }

    char* new = malloc(strlen(old) + TAB_SIZE + 1);

    if (new == NULL) {
        free(tab_str);
        return;
    }

    strncpy(new, old, pos);
    memcpy(new + pos, tab_str, TAB_SIZE);
    strcpy(new + pos + TAB_SIZE, old + pos);

    state->lines[state->cursor_y + state->scroll_offset] = new;
    free(old);
    free(tab_str);

    state->cursor_x += TAB_SIZE;
}

void delete_char(struct Editor_State* state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x - margin; /* X axis of the cursor excluding the margin */
    int y_pos = state->scroll_offset + state->cursor_y;

    int len = strlen(state->lines[y_pos]); /* Current line length */

    if (pos > 0) {  /* If not at the beggining of the line then delete character from an array of lines */
        for (int i = pos - 1; i < len; i++) {
            state->lines[y_pos][i] = state->lines[y_pos][i + 1];
        }
        state->cursor_x--;
    } else if (y_pos > 0) { /* If cursor is at the beggining of the line then delete current line */
        int size = strlen(state->lines[y_pos - 1]) + strlen(state->lines[y_pos]) + 1; /* Size of the previous line + current line + \0 (1) */
        char* buf = malloc(size);
        int previous_len = strlen(state->lines[y_pos - 1]) + margin;

        if (!buf) {
            return;
        }

        strcpy(buf, state->lines[y_pos - 1]);
        strcat(buf, state->lines[y_pos]);
        
        state->lines[y_pos - 1] = buf;

        for (int i = y_pos; i < state->total_lines; i++) {
            state->lines[i] = state->lines[i + 1];
        }

        state->total_lines--;
        state->cursor_x = previous_len;
        state->cursor_y--;
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

    if (pos > current_len) {
        pos = current_len;
    }

    if (!left || !right) {
        return;
    }

    strncpy(left, current, pos);
    left[pos] = '\0';

    strcpy(right, current + pos);

    free(state->lines[y_pos]);
    state->lines[y_pos] = NULL;

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
