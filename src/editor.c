#include "editor.h"
#include "select.h"
#include "ui/status_bar.h"
#include "utils/common.h"
#include "utils/fileio.h"

void init_editor(struct Editor_State *state) {
    state->is_saved = true;

    if (load_pos(state) == 0) {
        state->cursor_x = int_len(state->total_lines) + 2;
        state->cursor_y = 0;
        state->x_offset = 0;
        state->y_offset = 0;
    }

    state->max_char = state->cursor_x + state->x_offset;

    if (state->total_lines == 0) {
        state->lines[0] = strdup("");
        state->total_lines = 1;
    }

    initscr();
    raw();
    set_escdelay(0);
    keypad(stdscr, true);
    noecho();
    curs_set(2);

    load_pos(state);

    if (has_colors() == FALSE) {
        endwin();
        printf("Error: Terminal does not support colors");
        exit(1);
    }

    start_color();
    use_default_colors();

    init_color(8, rgb_to_ncurses(111), rgb_to_ncurses(118),
               rgb_to_ncurses(125));
    init_color(9, rgb_to_ncurses(15), rgb_to_ncurses(17), rgb_to_ncurses(22));
    init_color(10, rgb_to_ncurses(142), rgb_to_ncurses(145),
               rgb_to_ncurses(154));
    init_color(11, rgb_to_ncurses(31), rgb_to_ncurses(48), rgb_to_ncurses(70));

    init_pair(1, 8, -1);   // Unactive text color
    init_pair(2, 10, 9);   // Status bar color
    init_pair(3, 255, 11); // Selected text
}

void draw_editor(struct Editor_State *state) {
    erase();

    int screen_width = getmaxx(stdscr);
    int screen_height = getmaxy(stdscr);

    int line_num_pos = 0;
    int margin = int_len(state->total_lines) + 2;

    for (int index = state->y_offset;
         index < state->y_offset + screen_height - 1 &&
         index < state->total_lines;
         index++) {
        char *line = state->lines[index];
        char *spaces =
            mult_char(' ', int_len(state->total_lines) - int_len(index + 1));

        // Draw line number
        if (index == state->cursor_y + state->y_offset) {
            attron(COLOR_PAIR(1));
            mvprintw(line_num_pos, 0, " %s%d ", spaces, index + 1);
            attroff(COLOR_PAIR(1));
        } else {
            mvprintw(line_num_pos, 0, " %s%d ", spaces, index + 1);
        }

        // Draw line content with wrapping
        int col = margin;

        for (int symb = state->x_offset;
             symb < (int)strlen(line) && symb < state->x_offset + screen_width;
             symb++) {
            int file_x = symb;
            int file_y = index;

            if (is_selected(file_y, file_x)) {
                attron(COLOR_PAIR(3));
                mvprintw(line_num_pos, col, "%c", line[symb]);
                attroff(COLOR_PAIR(3));
            } else {
                mvprintw(line_num_pos, col, "%c", line[symb]);
            }
            col++;
        }

        line_num_pos++;
        if (line_num_pos >= screen_height - 1) {
            break; // Don't draw beyond screen bottom
        }
    }

    attron(COLOR_PAIR(2));
    draw_status_bar(state);
    attroff(COLOR_PAIR(2));

    move(state->cursor_y, state->cursor_x);
}

void move_cursor(int key, struct Editor_State *state, bool is_selecting) {
    int screen_width = getmaxx(stdscr);
    int screen_height = getmaxy(stdscr);
    int margin =
        int_len(state->total_lines) + 2; // Line number length + 2 spaces
    int line_len;

    switch (key) {
    case KEY_UP: {
        if (state->cursor_y + state->y_offset > 0) {
            if (state->cursor_y < 4 && state->y_offset > 0) {
                state->y_offset--;
            } else {
                state->cursor_y--;
            }
            state->cursor_x = state->max_char;
        } else {
            state->cursor_x = margin;
            state->max_char = state->cursor_x + state->x_offset;
        }

        line_len = strlen(state->lines[state->cursor_y + state->y_offset]);

        // Cursor clamping
        if (state->cursor_x - margin > line_len - state->x_offset ||
            state->cursor_x + 5 > screen_width) {
            state->cursor_x = margin + line_len;

            if (state->cursor_x + 5 > screen_width) {
                state->x_offset = state->cursor_x - screen_width + 5;
                state->cursor_x -= state->x_offset;
            } else if (state->cursor_x - margin < state->x_offset + 5 &&
                       state->x_offset > 0) {
                state->x_offset = line_len - (state->cursor_x - margin);
            } else {
                state->cursor_x = margin + line_len - state->x_offset;
            }
        }

        if (!is_selecting) {
            cancel_selection();
        }
        break;
    }

    case KEY_DOWN: {
        if (state->cursor_y < state->total_lines - state->y_offset - 1) {
            if (state->cursor_y > screen_height - 6 &&
                state->y_offset + screen_height < state->total_lines + 1) {
                state->y_offset++;
            } else {
                state->cursor_y++;
            }
            state->cursor_x = state->max_char;
        } else {
            line_len = strlen(state->lines[state->cursor_y + state->y_offset]);
            state->cursor_x = margin + line_len;
            state->max_char = state->cursor_x + state->x_offset;
        }

        line_len = strlen(state->lines[state->cursor_y + state->y_offset]);

        // Cursor clamping
        if (state->cursor_x - margin > line_len - state->x_offset ||
            state->cursor_x + 5 > screen_width) {
            state->cursor_x = margin + line_len;

            if (state->cursor_x + 5 > screen_width) {
                state->x_offset = state->cursor_x - screen_width + 5;
                state->cursor_x -= state->x_offset;
            } else if (state->cursor_x - margin < state->x_offset + 5 &&
                       state->x_offset > 0) {
                state->x_offset = line_len - (state->cursor_x - margin);
            } else {
                state->cursor_x = margin + line_len - state->x_offset;
            }
        }

        if (!is_selecting) {
            cancel_selection();
        }
        break;
    }

    case KEY_LEFT: {
        if (state->cursor_x > margin - state->x_offset) {
            if (state->cursor_x < 6 && state->x_offset > 0) {
                state->x_offset--;
            } else {
                state->cursor_x--;
            }
        } else if (state->cursor_x <= margin - state->x_offset &&
                   state->cursor_y + state->y_offset >= 1) {
            state->cursor_y--;
            line_len = strlen(state->lines[state->cursor_y + state->y_offset]);
            if (line_len + margin > screen_width - 5) {
                state->x_offset = (line_len + margin) - screen_width + 4;
            }
            state->cursor_x = margin + line_len - state->x_offset;
        }
        state->max_char = state->cursor_x + state->x_offset;

        if (!is_selecting) {
            cancel_selection();
        }
        break;
    }

    case KEY_RIGHT: {
        line_len = strlen(state->lines[state->cursor_y + state->y_offset]);

        if (state->cursor_x + state->x_offset < margin + line_len) {
            if (state->cursor_x + state->x_offset > screen_width - 5) {
                state->x_offset++;
            } else {
                state->cursor_x++;
            }
        } else if (state->cursor_x + margin + state->x_offset > line_len - 1 &&
                   state->cursor_y + state->y_offset < state->total_lines - 1) {
            state->cursor_y++;
            state->cursor_x = margin;
            state->x_offset = 0;
        }
        state->max_char = state->cursor_x + state->x_offset;

        if (!is_selecting) {
            cancel_selection();
        }
        break;
    }
    }
}

void handle_key(int key, struct Editor_State *state) {
    int margin = int_len(state->total_lines) + 2;

    if (key == 393) { // Shift + RIGHT_ARROW (Right arrow selection)
        start_selection(state->cursor_y + state->y_offset,
                        state->cursor_x - margin + state->x_offset);
        move_cursor(260, state, true);
        update_selection(state->cursor_y + state->y_offset,
                         state->cursor_x - margin + state->x_offset);
        return;
    }

    if (key == 402) { // Shift + LEFT_ARROW (Left arrow selection)
        start_selection(state->cursor_y + state->y_offset,
                        state->cursor_x - margin + state->x_offset);
        move_cursor(261, state, true);
        update_selection(state->cursor_y + state->y_offset,
                         state->cursor_x - margin + state->x_offset);
        return;
    }

    if (key == 337) { // Shift + UP_ARROW (Up arrow selection)
        start_selection(state->cursor_y + state->y_offset,
                        state->cursor_x - margin + state->x_offset);
        move_cursor(259, state, true);
        update_selection(state->cursor_y + state->y_offset,
                         state->cursor_x - margin + state->x_offset);
        return;
    }

    if (key == 336) { // Shift + DOWN_ARROW (Down arrow selection)
        start_selection(state->cursor_y + state->y_offset,
                        state->cursor_x - margin + state->x_offset);
        move_cursor(258, state, true);
        update_selection(state->cursor_y + state->y_offset,
                         state->cursor_x - margin + state->x_offset);
        return;
    }

    if (key == 27) { // Esc
        cancel_selection();
        return;
    }

    if (key == 17) { // CTRL + Q (Quit the editor)
        if (!state->is_saved) {
            ask_for_save(state);
            return;
        }
        save_pos(state);
        endwin();
        exit(0);
        return;
    }

    if ((key >= 32 && key <= 126)) {
        insert_char((char)key, state);
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
        int x_pos = state->cursor_x + state->x_offset - margin;
        int y_pos = state->cursor_y + state->y_offset;

        Point start_select;
        Point end_select;

        if (!is_selecting()) {
            start_select.x = x_pos - 1;
            start_select.y = y_pos;

            end_select.x = x_pos - 1;
            end_select.y = y_pos;
        } else if (is_selecting() &&
                   ((get_start().x > x_pos && get_start().y == y_pos) ||
                    (get_start().y > y_pos))) {
            start_select.x = x_pos;
            start_select.y = y_pos;

            end_select.x = get_start().x;
            end_select.y = get_start().y;
        } else if (is_selecting() &&
                   ((get_start().x < x_pos && get_start().y == y_pos) ||
                    (get_start().y < y_pos))) {
            start_select.x = get_start().x;
            start_select.y = get_start().y;

            end_select.x = x_pos;
            end_select.y = y_pos;
        } else {
            return;
        }

        deletion(state, start_select, end_select);
        state->is_saved = false;
        return;
    }

    if (key == KEY_ENTER || key == 10) {
        new_line(state);
        state->is_saved = false;
        return;
    }

    if (key == 19) { // CTRL + S (Save file)
        save_file(state);
        return;
    }

    if (key == 7) { // CTRL + G (Goto line)
        int target = goto_line(state);
        if (target != -1) {
            if (target < getmaxy(stdscr) - 1) {
                state->y_offset = 0;
            } else if (target > getmaxy(stdscr) - 2) {
                state->y_offset = target - (getmaxy(stdscr) / 2);
            }
            state->cursor_y = target - state->y_offset;
        }
        return;
    }

    if (key == 534 &&
        state->cursor_y + state->y_offset <
            state->total_lines +
                1) { // CTRL + DOWN_ARROW (Jump to the end of the file)
        if (state->total_lines > getmaxy(stdscr) - 1) {
            state->y_offset = state->total_lines - getmaxy(stdscr) + 1;
        }
        state->cursor_y = state->total_lines - state->y_offset - 1;
    }

    if (key == 575) { // CTRL + UP_ARROW (Jump to the beggining of the file)
        state->y_offset = 0;
        state->cursor_y = 0;
    }
}

int goto_line(struct Editor_State *state) {
    const int height = 3;
    const int width = 30;

    WINDOW *box = newwin(height, width, 1, getmaxx(stdscr) - 33);

    box(box, 0, 0);
    mvwprintw(box, 1, 2, "Go to line: ");

    curs_set(1);

    char input[15] = {'\0'};
    int pos = 0;

    int ch;
    while ((ch = wgetch(box)) != '\n') {
        if (ch == 17) {
            if (!state->is_saved) {
                ask_for_save(state);
            }
            save_pos(state);
            endwin();
            exit(0);
            return 0;
        }

        if (ch == 27) {
            delwin(box);
            return -1;
        } else if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') &&
                   pos > 0) {
            input[--pos] = '\0';
            mvwprintw(box, 1, 14 + pos, " ");
            wmove(box, 1, 14 + pos);
        } else if (isdigit(ch) && pos < (int)sizeof(input) - 1) {
            input[pos++] = ch;
            mvwprintw(box, 1, 13 + pos, "%c", ch);
            wmove(box, 1, 14 + pos);
        }
        wrefresh(box);
    }

    delwin(box);

    int line = atoi(input);
    if (line < 1) {
        return -1;
    }

    if (line > state->total_lines) {
        return state->total_lines - 1;
    }
    return line - 1;
}

void ask_for_save(struct Editor_State *state) {
    move(getmaxy(stdscr) - 1, 0);
    clrtoeol();

    attron(A_BOLD);
    mvwprintw(stdscr, getmaxy(stdscr) - 1, 0, "Save changes? (y/n): ");
    attroff(A_BOLD);

    curs_set(1);

    char input[15] = {'\0'};
    int pos = 0;

    int ch;
    while ((ch = wgetch(stdscr)) != '\n') {
        if (ch == 27) {
            return;
        } else if ((ch == KEY_BACKSPACE || ch == 127) && pos > 0) {
            input[--pos] = '\0';
            mvwprintw(stdscr, getmaxy(stdscr) - 1, 21 + pos, " ");
            wmove(stdscr, getmaxy(stdscr) - 1, 21 + pos);
        } else if (pos < (int)sizeof(input) - 1 && ch != KEY_BACKSPACE &&
                   ch != 127) {
            input[pos++] = ch;
            mvwprintw(stdscr, getmaxy(stdscr) - 1, 20 + pos, "%c", ch);
            wmove(stdscr, getmaxy(stdscr) - 1, 21 + pos);
        }
        wrefresh(stdscr);
    }

    noecho();

    if (tolower(input[0]) == 121) { // 'Y' or 'y'
        save_file(state);
        save_pos(state);
    }

    if (tolower(input[0]) == 121 || tolower(input[0]) == 110) {
        endwin();
        exit(0);
    }

    ask_for_save(state);
}

void insert_char(char c, struct Editor_State *state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x + state->x_offset - margin;

    char *old = state->lines[state->cursor_y + state->y_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
        state->lines[state->cursor_y + state->y_offset] = old;
    }

    char *new = malloc(strlen(old) + 2);
    memset(new, 0, strlen(old) + 2);

    if (new == NULL) {
        return;
    }

    strncpy(new, old, pos);
    new[pos] = c;
    strcpy(new + pos + 1, old + pos);

    state->lines[state->cursor_y + state->y_offset] = new;

    free(old);
    move_cursor(KEY_RIGHT, state, false);
}

void add_tab(struct Editor_State *state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x - margin;

    char *old = state->lines[state->cursor_y + state->y_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
        state->lines[state->cursor_y + state->y_offset] = old;
    }

    char *tab_str = mult_char(' ', TAB_SIZE);

    if (!tab_str) {
        return;
    }

    char *new = malloc(strlen(old) + TAB_SIZE + 1);

    if (new == NULL) {
        free(tab_str);
        return;
    }

    strncpy(new, old, pos);
    memcpy(new + pos, tab_str, TAB_SIZE);
    strcpy(new + pos + TAB_SIZE, old + pos);

    state->lines[state->cursor_y + state->y_offset] = new;
    free(old);
    free(tab_str);

    state->cursor_x += TAB_SIZE;
}

void deletion(struct Editor_State *state, Point start, Point end) {
    if (!is_selecting()) {
        int remove_count = end.x - start.x + 1;
        int length = strlen(state->lines[start.y]);
        
        memmove(&state->lines[start.y][start.x], &state->lines[end.y][end.x + 1], length - end.x);
        state->lines[start.y][length - remove_count] = '\0';

        state->cursor_x--;
    }

    cancel_selection();
}

void new_line(struct Editor_State *state) {
    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x + state->x_offset - margin;
    int y_pos = state->cursor_y + state->y_offset;

    char *current = state->lines[y_pos];
    int current_len = strlen(current);

    char *left = malloc(pos + 1);
    char *right = malloc(current_len - pos + 1);

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

    state->total_lines++;

    state->cursor_y++;
    margin = int_len(state->total_lines) + 2;
    state->cursor_x = margin;
    state->x_offset = 0;
}
