#include "file_tree.h"

#include <curses.h>
#include <stdlib.h>
#include <stdio.h>

void draw_tree(char **files, int el_num) {
    initscr();

    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *tree_win = newwin(max_y, max_x, 0, 0);

    noecho();

    int ch;
    while ((ch = getch()) != 27) {
        wclear(tree_win);
        for (int i = 0; i < el_num; i++) {
            mvwprintw(tree_win, i, 0, "%s", files[i]);
        }
        wrefresh(tree_win);
    }

    delwin(tree_win);
    endwin();
    exit(0);
}
