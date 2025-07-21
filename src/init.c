#include "init.h"

#include <string.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#include "editor.h"
#include "utils.h"
#include "cursor.h"

void init_editor(void) {
    editor.is_saved = true;
    editor.bottom_text = "";

    if (editor.total_lines == 0) {
        editor.lines[0] = strdup("");
        editor.total_lines++;
    }

    editor.margin = int_len(editor.total_lines) + 2;

    cursor.x = editor.margin;
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
}

void init_colors() {
    if (has_colors() == FALSE) {
        endwin();
        printf("Error: terminal does not support colors");
        exit(0);
    }

    start_color();
    use_default_colors();

    /* Custom colors */
    init_color(COLOR_DEFAULT, rgb_to_ncurses(255), rgb_to_ncurses(255), rgb_to_ncurses(255));
    init_color(COLOR_KEYWORD, rgb_to_ncurses(238), rgb_to_ncurses(130), rgb_to_ncurses(119));
    init_color(COLOR_TYPE, rgb_to_ncurses(137), rgb_to_ncurses(190), rgb_to_ncurses(250));
    init_color(COLOR_STRING, rgb_to_ncurses(145), rgb_to_ncurses(176), rgb_to_ncurses(209));
    init_color(COLOR_NUMBER, rgb_to_ncurses(124), rgb_to_ncurses(170), rgb_to_ncurses(224));
    init_color(COLOR_CHAR, rgb_to_ncurses(114), rgb_to_ncurses(158), rgb_to_ncurses(207));
    init_color(COLOR_FUNCTION, rgb_to_ncurses(203), rgb_to_ncurses(170), rgb_to_ncurses(249));
    init_color(COLOR_PREPROCESSOR, rgb_to_ncurses(213), rgb_to_ncurses(117), rgb_to_ncurses(108));
    init_color(COLOR_UNACTIVE, rgb_to_ncurses(126), rgb_to_ncurses(132), rgb_to_ncurses(141));
    init_color(COLOR_STATUS_BAR, rgb_to_ncurses(34), rgb_to_ncurses(38), rgb_to_ncurses(44));
    init_color(COLOR_BACKGROUND, rgb_to_ncurses(14), rgb_to_ncurses(17), rgb_to_ncurses(22));

    init_pair(PAIR_DEFAULT, COLOR_DEFAULT, -1);
    init_pair(PAIR_KEYWORD, COLOR_KEYWORD, -1);
    init_pair(PAIR_TYPE, COLOR_TYPE, -1);
    init_pair(PAIR_STRING, COLOR_STRING, -1);
    init_pair(PAIR_NUM, COLOR_NUMBER, -1);
    init_pair(PAIR_CHAR, COLOR_CHAR, -1);
    init_pair(PAIR_FUNCTION, COLOR_FUNCTION, -1);
    init_pair(PAIR_PREPROCESSOR, COLOR_PREPROCESSOR, -1);
    init_pair(PAIR_UNACTIVE, COLOR_UNACTIVE, -1);
    init_pair(PAIR_STATUS_BAR, COLOR_UNACTIVE, COLOR_STATUS_BAR);
    init_pair(PAIR_BACKGROUND, COLOR_DEFAULT, COLOR_BACKGROUND);

    clear();

    wbkgd(stdscr, COLOR_PAIR(PAIR_BACKGROUND));

    const int max_x = getmaxx(stdscr);
    const int max_y = getmaxy(stdscr);

    for (int y = 0; y < max_x; y++) {
        for (int x = 0; x < max_y; x++) {
            mvaddch(y, x, ' ');
        }
    }

    refresh();
}

void init_indent(char *line, bool *indent_measured) {
    int indent_count = 0;
    editor.tab_width = DEFAULT_INDENT_SIZE;

    if (line[0] == '\t') {
        editor.tab_indent = true;
        editor.indent_size = 1;
        *indent_measured = true;
        return;
    }
    
    if ((line[0] == ' ' && line[1] == ' ') || line[0] == '\t') {
        for (int i = 0; i < (int) strlen(line); i++) {
            if (line[i] == ' ') {
                indent_count++;
            } else {
                break;
            }
        }
        editor.tab_indent = false;
        editor.indent_size = indent_count;
        *indent_measured = true;
        return;
    }
}
