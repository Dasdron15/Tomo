#include <curses.h>

#include "command_palette.h"
#include "themes.h"

void draw_command_palette(void) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    int height = 6;
    int width = max_x - 20;
    int startx = (max_x - width) / 2;
    int starty = (max_y - height) / 2;

    WINDOW *palette = newwin(height, width, starty, startx);
    keypad(palette, TRUE);
    wbkgd(palette, COLOR_PAIR(PAIR_BACKGROUND));

    const char *options[] = {"Change theme", "Save file", "Exit"};
    int n_options = sizeof(options) / sizeof(options[0]);
    int highlight = 0;

    while (1) {
        getmaxyx(stdscr, max_y, max_x);
        width = max_x - 20;
        height = n_options + 2;
        startx = (max_x - width) / 2;
        starty = (max_y - height) / 2;
        wresize(palette, height, width);
        mvwin(palette, starty, startx);

        werase(palette);
        box(palette, 0, 0);

        for (int i = 0; i < n_options; i++) {
            if (i == highlight) {
                wattron(palette, COLOR_PAIR(PAIR_STATUS_BAR));
            }
            mvwprintw(palette, i + 1, 2, "%s", options[i]);
            wattroff(palette, COLOR_PAIR(PAIR_STATUS_BAR));
        }

        wrefresh(palette);
        wrefresh(stdscr);

        int ch = wgetch(palette);
        if (ch == KEY_UP) {
            highlight = (highlight - 1 + n_options) % n_options;
        } else if (ch == KEY_DOWN) {
            highlight = (highlight + 1) % n_options;
        } else if (ch == '\n') {
            delwin(palette);
            break;
        } else if (ch == 27) {
            delwin(palette);
            break;
        }
    }
}
