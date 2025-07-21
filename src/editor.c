#include "editor.h"

#include <stdlib.h>
#include <string.h>

#include <curses.h>

#include "common.h"
#include "cursor.h"
#include "draw.h"
#include "fileio.h"

EditorState editor;

void init_editor(void) {
    editor.is_saved = true;
    editor.bottom_text = "";

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

    printf("\033[5 q"); // Set cursor to line
    initscr();
    raw();
    set_escdelay(0);
    keypad(stdscr, true);
    noecho();

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
    init_color(12, rgb_to_ncurses(130), rgb_to_ncurses(180), rgb_to_ncurses(235));

    init_color(13, rgb_to_ncurses(238), rgb_to_ncurses(130), rgb_to_ncurses(119));

    init_color(14, rgb_to_ncurses(137), rgb_to_ncurses(190), rgb_to_ncurses(250));

    init_color(15,rgb_to_ncurses(182), rgb_to_ncurses(152), rgb_to_ncurses(223));

    init_pair(1, 8, -1);   // Unactive text color
    init_pair(2, 10, 9);   // Status bar color
    init_pair(3, 255, 11); // Selected text
    init_pair(4, 255, -1); // Default text color
    init_pair(5, 12, -1); // Numbers color
    init_pair(6, 13, -1); // Keyword color
    init_pair(7, 14, -1); // Type color
    init_pair(8, 15, -1);

    // Disable default terminal text selection
    printf("\033[?1006h");
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);
}

void draw_editor() {
    erase();

    draw_text_area();
    draw_status_bar();
    draw_bottom_text();
    place_cursor();
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
            editor.bottom_text = "";
            return;
        } else if ((ch == KEY_BACKSPACE || ch == 127) && pos > 0) {
            input[--pos] = '\0';
            mvwprintw(stdscr, getmaxy(stdscr) - 1, 21 + pos, " ");
            wmove(stdscr, getmaxy(stdscr) - 1, 21 + pos);
        } else if (pos < (int)sizeof(input) - 1 && ch < 127 && ch > 31) {
            input[pos++] = ch;
            mvwprintw(stdscr, getmaxy(stdscr) - 1, 20 + pos, "%c", ch);
            wmove(stdscr, getmaxy(stdscr) - 1, 21 + pos);
        }
        wrefresh(stdscr);
    }

    if (strcasecmp(input, "y") == 0 || strcasecmp(input, "yes") == 0) {
        reset();
        save_file();
        endwin();
        exit(0);
        return;
    }

    if (strcasecmp(input, "n") == 0 || strcasecmp(input, "no")) {
        reset();
        endwin();
        exit(0);
        return;
    }
}

void reset() {
    printf("\033[?1006l"); // Reset mouse selection

    printf("\033[2 q");
    fflush(stdout);
}
