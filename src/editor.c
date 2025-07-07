#include "editor.h"

#include <stdlib.h>
#include <string.h>

#include <curses.h>
#include <strings.h>

#include "common.h"
#include "cursor.h"
#include "fileio.h"
#include "select.h"
#include "status_bar.h"

EditorState editor;

void init_editor(void) {
    editor.is_saved = true;

    if (editor.total_lines == 0) {
        editor.lines[0] = strdup("");
        editor.total_lines = 1;
    }

    editor.margin = int_len(editor.total_lines) + 2;

    cursor.x = editor.margin;
    cursor.max_x = editor.margin;
    cursor.y = 0;
    cursor.x_offset = 0;
    cursor.y_offset = 0;

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

    // Disable default terminal text selection
    printf("\033[?1006h");
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);
}

void draw_editor() {
    erase();

    int screen_width = getmaxx(stdscr);
    int screen_height = getmaxy(stdscr);

    int line_num_pos = 0;
    int margin = int_len(editor.total_lines) + 2;

    for (int index = cursor.y_offset;
         index < cursor.y_offset + screen_height - 1 &&
         index < editor.total_lines;
         index++) {
        char *line = editor.lines[index];
        char *spaces =
            mult_char(' ', int_len(editor.total_lines) - int_len(index + 1));

        // Draw line number
        if (index == cursor.y + cursor.y_offset) {
            attron(COLOR_PAIR(1));
            mvprintw(line_num_pos, 0, " %s%d ", spaces, index + 1);
            attroff(COLOR_PAIR(1));
        } else {
            mvprintw(line_num_pos, 0, " %s%d ", spaces, index + 1);
        }

        // Draw line content with wrapping
        int col = margin;

        for (size_t symb = cursor.x_offset;
             symb <= strlen(line) && symb < cursor.x_offset + screen_width;
             symb++) {
            int file_x = symb;
            int file_y = index;

            char ch = (symb < strlen(line)) ? line[symb] : ' ';

            if (is_selected(file_y, file_x)) {
                attron(COLOR_PAIR(3));
                mvprintw(line_num_pos, col, "%c", ch);
                attroff(COLOR_PAIR(3));
            } else {
                mvprintw(line_num_pos, col, "%c", ch);
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

    move(cursor.y, cursor.x);
}

void ask_for_save() {
    move(getmaxy(stdscr) - 1, 0);
    clrtoeol();

    attron(A_BOLD);
    mvwprintw(stdscr, getmaxy(stdscr) - 1, 0, "Save changes? (y/n): ");
    attroff(A_BOLD);

    curs_set(1);

    char input[16] = {0};
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

    if (strcasecmp(input, "y") == 0 || strcasecmp(input, "yes") == 0) {
        save_file();
    }

    if (strcasecmp(input, "n") == 0 || strcasecmp(input, "no")) {
        endwin();
        exit(0);
    }

    ask_for_save();
}
