#include "editor.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <curses.h>

#include "common.h"
#include "fileio.h"
#include "select.h"
#include "status_bar.h"

void init_editor(void) {
    editor.is_saved = true;

    editor.cursor_x = int_len(editor.total_lines) + 2;
    editor.cursor_y = 0;
    editor.x_offset = 0;
    editor.y_offset = 0;

    editor.max_char = editor.cursor_x + editor.x_offset;

    if (editor.total_lines == 0) {
        editor.lines[0] = strdup("");
        editor.total_lines = 1;
    }

    initscr();
    raw();
    set_escdelay(0);
    keypad(stdscr, true);
    noecho();
    curs_set(2);

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

void draw_editor() {
    erase();

    int screen_width = getmaxx(stdscr);
    int screen_height = getmaxy(stdscr);

    int line_num_pos = 0;
    int margin = int_len(editor.total_lines) + 2;

    for (int index = editor.y_offset;
         index < editor.y_offset + screen_height - 1 &&
         index < editor.total_lines;
         index++) {
        char *line = editor.lines[index];
        char *spaces =
            mult_char(' ', int_len(editor.total_lines) - int_len(index + 1));

        // Draw line number
        if (index == editor.cursor_y + editor.y_offset) {
            attron(COLOR_PAIR(1));
            mvprintw(line_num_pos, 0, " %s%d ", spaces, index + 1);
            attroff(COLOR_PAIR(1));
        } else {
            mvprintw(line_num_pos, 0, " %s%d ", spaces, index + 1);
        }

        // Draw line content with wrapping
        int col = margin;

        for (int symb = editor.x_offset;
             symb < (int)strlen(line) && symb < editor.x_offset + screen_width;
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
    draw_status_bar();
    attroff(COLOR_PAIR(2));

    move(editor.cursor_y, editor.cursor_x);
}

void move_cursor(int key, bool is_selecting) {
    int screen_width = getmaxx(stdscr);
    int screen_height = getmaxy(stdscr);
    int margin =
        int_len(editor.total_lines) + 2; // Line number length + 2 spaces
    int line_len;

    switch (key) {
    case KEY_UP: {
        if (editor.cursor_y + editor.y_offset > 0) {
            if (editor.cursor_y < 4 && editor.y_offset > 0) {
                editor.y_offset--;
            } else {
                editor.cursor_y--;
            }
            editor.cursor_x = editor.max_char;
        } else {
            editor.cursor_x = margin;
            editor.max_char = editor.cursor_x + editor.x_offset;
        }

        line_len = strlen(editor.lines[editor.cursor_y + editor.y_offset]);

        // Cursor clamping
        if (editor.cursor_x - margin > line_len - editor.x_offset ||
            editor.cursor_x + 5 > screen_width) {
            editor.cursor_x = margin + line_len;

            if (editor.cursor_x + 5 > screen_width) {
                editor.x_offset = editor.cursor_x - screen_width + 5;
                editor.cursor_x -= editor.x_offset;
            } else if (editor.cursor_x - margin < editor.x_offset + 5 &&
                       editor.x_offset > 0) {
                editor.x_offset = line_len - (editor.cursor_x - margin);
            } else {
                editor.cursor_x = margin + line_len - editor.x_offset;
            }
        }

        if (!is_selecting) {
            cancel_selection();
        }
        break;
    }

    case KEY_DOWN: {
        if (editor.cursor_y < editor.total_lines - editor.y_offset - 1) {
            if (editor.cursor_y > screen_height - 6 &&
                editor.y_offset + screen_height < editor.total_lines + 1) {
                editor.y_offset++;
            } else {
                editor.cursor_y++;
            }
            editor.cursor_x = editor.max_char;
        } else {
            line_len = strlen(editor.lines[editor.cursor_y + editor.y_offset]);
            editor.cursor_x = margin + line_len;
            editor.max_char = editor.cursor_x + editor.x_offset;
        }

        line_len = strlen(editor.lines[editor.cursor_y + editor.y_offset]);

        // Cursor clamping
        if (editor.cursor_x - margin > line_len - editor.x_offset ||
            editor.cursor_x + 5 > screen_width) {
            editor.cursor_x = margin + line_len;

            if (editor.cursor_x + 5 > screen_width) {
                editor.x_offset = editor.cursor_x - screen_width + 5;
                editor.cursor_x -= editor.x_offset;
            } else if (editor.cursor_x - margin < editor.x_offset + 5 &&
                       editor.x_offset > 0) {
                editor.x_offset = line_len - (editor.cursor_x - margin);
            } else {
                editor.cursor_x = margin + line_len - editor.x_offset;
            }
        }

        if (!is_selecting) {
            cancel_selection();
        }
        break;
    }

    case KEY_LEFT: {
        if (editor.cursor_x > margin - editor.x_offset) {
            if (editor.cursor_x < 6 && editor.x_offset > 0) {
                editor.x_offset--;
            } else {
                editor.cursor_x--;
            }
        } else if (editor.cursor_x <= margin - editor.x_offset &&
                   editor.cursor_y + editor.y_offset >= 1) {
            editor.cursor_y--;
            line_len = strlen(editor.lines[editor.cursor_y + editor.y_offset]);
            if (line_len + margin > screen_width - 5) {
                editor.x_offset = (line_len + margin) - screen_width + 4;
            }
            editor.cursor_x = margin + line_len - editor.x_offset;
        }
        editor.max_char = editor.cursor_x + editor.x_offset;

        if (!is_selecting) {
            cancel_selection();
        }
        break;
    }

    case KEY_RIGHT: {
        line_len = strlen(editor.lines[editor.cursor_y + editor.y_offset]);

        if (editor.cursor_x + editor.x_offset < margin + line_len) {
            if (editor.cursor_x + editor.x_offset > screen_width - 5) {
                editor.x_offset++;
            } else {
                editor.cursor_x++;
            }
        } else if (editor.cursor_x + margin + editor.x_offset > line_len - 1 &&
                   editor.cursor_y + editor.y_offset < editor.total_lines - 1) {
            editor.cursor_y++;
            editor.cursor_x = margin;
            editor.x_offset = 0;
        }
        editor.max_char = editor.cursor_x + editor.x_offset;

        if (!is_selecting) {
            cancel_selection();
        }
        break;
    }
    }
}

void handle_key(int key) {
    int margin = int_len(editor.total_lines) + 2;

    if (key == 393) { // Shift + RIGHT_ARROW (Right arrow selection)
        start_selection(editor.cursor_y + editor.y_offset,
                        editor.cursor_x - margin + editor.x_offset);
        move_cursor(260, true);
        update_selection(editor.cursor_y + editor.y_offset,
                         editor.cursor_x - margin + editor.x_offset);
        return;
    }

    if (key == 402) { // Shift + LEFT_ARROW (Left arrow selection)
        start_selection(editor.cursor_y + editor.y_offset,
                        editor.cursor_x - margin + editor.x_offset);
        move_cursor(261, true);
        update_selection(editor.cursor_y + editor.y_offset,
                         editor.cursor_x - margin + editor.x_offset);
        return;
    }

    if (key == 337) { // Shift + UP_ARROW (Up arrow selection)
        start_selection(editor.cursor_y + editor.y_offset,
                        editor.cursor_x - margin + editor.x_offset);
        move_cursor(259, true);
        update_selection(editor.cursor_y + editor.y_offset,
                         editor.cursor_x - margin + editor.x_offset);
        return;
    }

    if (key == 336) { // Shift + DOWN_ARROW (Down arrow selection)
        start_selection(editor.cursor_y + editor.y_offset,
                        editor.cursor_x - margin + editor.x_offset);
        move_cursor(258, true);
        update_selection(editor.cursor_y + editor.y_offset,
                         editor.cursor_x - margin + editor.x_offset);
        return;
    }

    if (key == 27) { // Esc
        cancel_selection();
        return;
    }

    if (key == 17) { // CTRL + Q (Quit the editor)
        if (!editor.is_saved) {
            ask_for_save();
            return;
        }
        endwin();
        exit(0);
        return;
    }

    if ((key >= 32 && key <= 126)) {
        insert_char((char)key);
        editor.is_saved = false;
        return;
    }

    if (key == '\t') {
        add_tab();
        editor.max_char = editor.cursor_x;
        editor.is_saved = false;
        return;
    }

    if (key == KEY_BACKSPACE || key == 127) {
        int x_pos = editor.cursor_x + editor.x_offset - margin;
        int y_pos = editor.cursor_y + editor.y_offset;

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

        deletion(start_select, end_select);
        editor.is_saved = false;
        return;
    }

    if (key == KEY_ENTER || key == 10) {
        new_line();
        editor.is_saved = false;
        return;
    }

    if (key == 19) { // CTRL + S (Save file)
        save_file();
        return;
    }

    if (key == 7) { // CTRL + G (Goto line)
        int target = goto_line();
        if (target != -1) {
            if (target < getmaxy(stdscr) - 1) {
                editor.y_offset = 0;
            } else if (target > getmaxy(stdscr) - 2) {
                editor.y_offset = target - (getmaxy(stdscr) / 2);
            }
            editor.cursor_y = target - editor.y_offset;
        }
        return;
    }

    if (key == 534 &&
        editor.cursor_y + editor.y_offset <
            editor.total_lines +
                1) { // CTRL + DOWN_ARROW (Jump to the end of the file)
        if (editor.total_lines > getmaxy(stdscr) - 1) {
            editor.y_offset = editor.total_lines - getmaxy(stdscr) + 1;
        }
        editor.cursor_y = editor.total_lines - editor.y_offset - 1;
    }

    if (key == 575) { // CTRL + UP_ARROW (Jump to the beggining of the file)
        editor.y_offset = 0;
        editor.cursor_y = 0;
    }
}

int goto_line() {
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
            if (!editor.is_saved) {
                ask_for_save();
            }
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

    if (line > editor.total_lines) {
        return editor.total_lines - 1;
    }
    return line - 1;
}

void ask_for_save() {
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
        save_file();
    }

    if (tolower(input[0]) == 121 || tolower(input[0]) == 110) {
        endwin();
        exit(0);
    }

    ask_for_save();
}

void insert_char(char c) {
    int margin = int_len(editor.total_lines) + 2;
    int pos = editor.cursor_x + editor.x_offset - margin;

    char *old = editor.lines[editor.cursor_y + editor.y_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
        editor.lines[editor.cursor_y + editor.y_offset] = old;
    }

    char *new = malloc(strlen(old) + 2);
    memset(new, 0, strlen(old) + 2);

    if (new == NULL) {
        return;
    }

    strncpy(new, old, pos);
    new[pos] = c;
    strcpy(new + pos + 1, old + pos);

    editor.lines[editor.cursor_y + editor.y_offset] = new;

    free(old);
    move_cursor(KEY_RIGHT, false);
}

void add_tab() {
    int margin = int_len(editor.total_lines) + 2;
    int pos = editor.cursor_x - margin;

    char *old = editor.lines[editor.cursor_y + editor.y_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
        editor.lines[editor.cursor_y + editor.y_offset] = old;
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

    editor.lines[editor.cursor_y + editor.y_offset] = new;
    free(old);
    free(tab_str);

    editor.cursor_x += TAB_SIZE;
}

void deletion(Point start, Point end) {
    int margin = int_len(editor.total_lines) + 2;

    if ((start.y == end.y && is_selecting()) ||
        (!is_selecting() && !(start.x < 0 && start.y == 0))) {
        if (start.x < 0 && start.y > 0) {
            move_cursor(KEY_LEFT, false);

            char *new_line = realloc(editor.lines[start.y - 1],
                                     strlen(editor.lines[start.y - 1]) +
                                         strlen(editor.lines[start.y]) + 1);
            editor.lines[start.y - 1] = new_line;

            strcat(editor.lines[start.y - 1], editor.lines[start.y]);

            for (int i = start.y; i < editor.total_lines; i++) {
                editor.lines[i] = editor.lines[i + 1];
            }

            editor.total_lines--;
            return;
        }

        int remove_count = end.x - start.x + 1;
        int length = strlen(editor.lines[start.y]);

        memmove(&editor.lines[start.y][start.x],
                &editor.lines[end.y][end.x + 1], length - end.x);
        editor.lines[start.y][length - remove_count] = '\0';

        editor.cursor_x = start.x + margin;
    } else if (!(start.x < 0 && start.y == 0)) {
        int deletion_range = end.y - start.y;

        if (start.x > 0) {
            editor.lines[start.y][start.x] = '\0';
        }

        char *line = editor.lines[end.y];
        memmove(line, line + end.x + 1, strlen(line) - end.x);

        char *new_line = realloc(editor.lines[start.y],
                                 strlen(editor.lines[start.y]) + strlen(line));
        editor.lines[start.y] = new_line;
        strcat(editor.lines[start.y], editor.lines[end.y]);

        for (int i = end.y + 1; i < editor.total_lines; i++) {
            editor.lines[start.y + 1 + (i - end.y - 1)] = editor.lines[i];
        }

        editor.total_lines -= deletion_range;

        editor.cursor_x = start.x + margin;
        editor.cursor_y = start.y;

        if (editor.cursor_y + editor.y_offset > start.y + 5) {
            endwin();
        }
    }

    cancel_selection();
}

void new_line() {
    int margin = int_len(editor.total_lines) + 2;
    int pos = editor.cursor_x + editor.x_offset - margin;
    int y_pos = editor.cursor_y + editor.y_offset;

    char *current = editor.lines[y_pos];
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

    free(editor.lines[y_pos]);
    editor.lines[y_pos] = NULL;

    for (int i = editor.total_lines; i > y_pos; i--) {
        editor.lines[i] = editor.lines[i - 1];
    }

    editor.lines[y_pos] = left;
    editor.lines[y_pos + 1] = right;

    editor.total_lines++;

    editor.cursor_y++;
    margin = int_len(editor.total_lines) + 2;
    editor.cursor_x = margin;
    editor.x_offset = 0;
}
