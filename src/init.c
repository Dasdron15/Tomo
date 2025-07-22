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
    init_color(COLOR_SELECT, rgb_to_ncurses(30), rgb_to_ncurses(64), rgb_to_ncurses(151));

    /* Default color pairs */
    init_pair(PAIR_DEFAULT, COLOR_DEFAULT, COLOR_BACKGROUND);
    init_pair(PAIR_KEYWORD, COLOR_KEYWORD, COLOR_BACKGROUND);
    init_pair(PAIR_TYPE, COLOR_TYPE, COLOR_BACKGROUND);
    init_pair(PAIR_STRING, COLOR_STRING, COLOR_BACKGROUND);
    init_pair(PAIR_NUM, COLOR_NUMBER, COLOR_BACKGROUND);
    init_pair(PAIR_CHAR, COLOR_CHAR, COLOR_BACKGROUND);
    init_pair(PAIR_FUNCTION, COLOR_FUNCTION, COLOR_BACKGROUND);
    init_pair(PAIR_PREPROCESSOR, COLOR_PREPROCESSOR, COLOR_BACKGROUND);
    init_pair(PAIR_UNACTIVE, COLOR_UNACTIVE, COLOR_BACKGROUND);
    init_pair(PAIR_STATUS_BAR, COLOR_UNACTIVE, COLOR_STATUS_BAR);
    init_pair(PAIR_BACKGROUND, COLOR_DEFAULT, COLOR_BACKGROUND);

    /* Selection color pairs */
    init_pair(PAIR_SELECT_DEFAULT, COLOR_DEFAULT, COLOR_SELECT);
    init_pair(PAIR_SELECT_KEYWORD, COLOR_KEYWORD, COLOR_SELECT);
    init_pair(PAIR_SELECT_TYPE, COLOR_TYPE, COLOR_SELECT);
    init_pair(PAIR_SELECT_STRING, COLOR_STRING, COLOR_SELECT);
    init_pair(PAIR_SELECT_NUM, COLOR_NUMBER, COLOR_SELECT);
    init_pair(PAIR_SELECT_CHAR, COLOR_CHAR, COLOR_SELECT);
    init_pair(PAIR_SELECT_FUNCTION, COLOR_FUNCTION, COLOR_SELECT);
    init_pair(PAIR_SELECT_PREPROCESSOR, COLOR_PREPROCESSOR, COLOR_SELECT);
    init_pair(PAIR_SELECT_UNACTIVE, COLOR_UNACTIVE, COLOR_SELECT);

    wbkgd(stdscr, COLOR_PAIR(PAIR_BACKGROUND));
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
