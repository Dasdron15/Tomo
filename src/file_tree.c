#include "file_tree.h"

#include <curses.h>
#include <stdlib.h>
#include <stdio.h>

#include "themes.h"
#include "editor.h"

void draw_tree(char **files, int el_num) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *tree_win = newwin(max_y, max_x, 0, 0);

    keypad(tree_win, TRUE);
    wbkgd(tree_win, COLOR_PAIR(PAIR_BACKGROUND));

    int pos = 0;
    int ch = 0;
    while (ch != 27) {
        wclear(tree_win);
        for (int i = 0; i < el_num; i++) {
            if (i == pos) {
                wattron(tree_win, COLOR_PAIR(PAIR_STATUS_BAR));
                mvwhline(tree_win, i, 0, ' ', max_x);
                mvwprintw(tree_win, i, 0, " %s", files[i]);
                wattroff(tree_win, COLOR_PAIR(PAIR_STATUS_BAR));
            } else {
                mvwprintw(tree_win, i, 0, " %s", files[i]);
            }
        }
        wrefresh(tree_win);
        ch = wgetch(tree_win);

        switch (ch) {
            case KEY_UP:
                if (pos > 0) pos--;
                else pos = el_num - 1;
                break;
            case KEY_DOWN:
                if (pos < el_num - 1) pos++;
                else pos = 0;
                break;
        }
    }

    delwin(tree_win);
    reset();
    endwin();
    exit(0);
}
