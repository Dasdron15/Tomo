#include "file_tree.h"

#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "themes.h"
#include "fileio.h"
#include "editor.h"

void draw_tree(char **files, int el_num, char *dir_path) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *tree_win = newwin(max_y, max_x, 0, 0);

    keypad(tree_win, TRUE);
    wbkgd(tree_win, COLOR_PAIR(PAIR_BACKGROUND));
    curs_set(0);

    int pos = 0;
    int ch = 0;
    while (ch != '\n') {
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

    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", dir_path, files[pos]);
    char *real_path = realpath(path, NULL);

    editor.filename = real_path;

    load_file(real_path);
}
