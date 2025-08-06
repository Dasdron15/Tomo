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
        printf("Error: terminal does not support colors\n");
        exit(0);
    }

    start_color();

    /* Custom colors */
    init_color(COLOR_DEFAULT, rgb_to_ncurses(255), rgb_to_ncurses(255), rgb_to_ncurses(255));
    init_color(COLOR_KEYWORD, rgb_to_ncurses(187), rgb_to_ncurses(137), rgb_to_ncurses(189));
    init_color(COLOR_TYPE, rgb_to_ncurses(103), rgb_to_ncurses(154), rgb_to_ncurses(209));
    init_color(COLOR_STRING, rgb_to_ncurses(197), rgb_to_ncurses(148), rgb_to_ncurses(124));
    init_color(COLOR_NUMBER, rgb_to_ncurses(186), rgb_to_ncurses(205), rgb_to_ncurses(171));
    init_color(COLOR_CHAR, rgb_to_ncurses(210), rgb_to_ncurses(187), rgb_to_ncurses(133));
    init_color(COLOR_FUNCTION, rgb_to_ncurses(220), rgb_to_ncurses(220), rgb_to_ncurses(175));
    init_color(COLOR_PREPROCESSOR, rgb_to_ncurses(188), rgb_to_ncurses(137), rgb_to_ncurses(189));
    init_color(COLOR_UNACTIVE, rgb_to_ncurses(111), rgb_to_ncurses(118), rgb_to_ncurses(128));
    init_color(COLOR_STATUS_BAR, rgb_to_ncurses(34), rgb_to_ncurses(38), rgb_to_ncurses(44));
    init_color(COLOR_BACKGROUND, rgb_to_ncurses(31), rgb_to_ncurses(31), rgb_to_ncurses(31));
    init_color(COLOR_SELECT, rgb_to_ncurses(30), rgb_to_ncurses(64), rgb_to_ncurses(151));
    init_color(COLOR_COMMENT, rgb_to_ncurses(116), rgb_to_ncurses(152), rgb_to_ncurses(93));

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
    init_pair(PAIR_COMMENT, COLOR_COMMENT, COLOR_BACKGROUND);

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
    init_pair(PAIR_SELECT_COMMENT, COLOR_COMMENT, COLOR_SELECT);

    bkgd(COLOR_PAIR(PAIR_BACKGROUND));
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
