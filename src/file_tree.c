#include "file_tree.h"

#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>
#include <string.h>

#include "themes.h"
#include "fileio.h"
#include "editor.h"

void draw_tree(char **files, int el_num, char *dir_path) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *tree_win = newwin(max_y, max_x, 0, 0);

    keypad(tree_win, TRUE);
    wbkgd(tree_win, COLOR_PAIR(PAIR_BACKGROUND));
    curs_set(1);
    char search_buffer[256] = {0};
    int search_pos = 0;

    char *filtered[1024];
    int filtered_count = 0;
    int pos = 0;
    int ch = 0;
    while (ch != '\n') {
        werase(tree_win);
        filtered_count = 0;
        for (int i = 0; i < el_num; i++) {
            if (search_buffer[0] == '\0' || strcasestr(files[i], search_buffer)) {
                filtered[filtered_count++] = files[i];
            }
        }
        if (pos >= filtered_count) pos = filtered_count - 1;
        if (pos < 0) pos = 0;

        for (int i = 0; i < filtered_count; i++) {
            if (i == pos) {
                wattron(tree_win, COLOR_PAIR(PAIR_STATUS_BAR));
                mvwhline(tree_win, i, 0, ' ', max_x);
                mvwprintw(tree_win, i, 0, " %s", filtered[i]);
                wattroff(tree_win, COLOR_PAIR(PAIR_STATUS_BAR));
            } else {
                mvwprintw(tree_win, i, 0, " %s", filtered[i]);
            }
        }

        mvwprintw(tree_win, max_y - 1, 0, "> %s", search_buffer);
        wrefresh(tree_win);
        ch = wgetch(tree_win);

        switch (ch) {
            case KEY_UP:
                if (pos > 0) pos--;
                else pos = filtered_count - 1;
                break;
            case KEY_DOWN:
                if (pos < filtered_count - 1) pos++;
                else pos = 0;
                break;

            case 17:
                delwin(tree_win);
                endwin();
                reset();
                exit(0);
            case KEY_BACKSPACE:
                if (search_pos > 0) {
                    search_buffer[--search_pos] = '\0';
                }
                break;
            default:
                if (ch > 31 && ch < 127 && search_pos < (int)sizeof(search_buffer) - 1) {
                    search_buffer[search_pos++] = ch;
                    search_buffer[search_pos] = '\0';
                }
                break;

        }
    }
    delwin(tree_win);

    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", dir_path, filtered[pos]);
    char *real_path = realpath(path, NULL);

    // Check if path is a directory
    struct stat path_stat;
    stat(real_path, &path_stat);

    curs_set(1);

    if (!S_ISREG(path_stat.st_mode)) {
        open_dir(real_path);
        return;
    }

    editor.filename = real_path;

    load_file(real_path);
}
