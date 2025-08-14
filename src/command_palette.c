#include <curses.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "command_palette.h"
#include "editor.h"
#include "themes.h"

int draw_command_palette(void) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    const char *options[] = {"Change theme", "Go to line", "Save file", "Exit"};
    int n_options = sizeof(options) / sizeof(options[0]);
    int highlight = 0;

    int pos = 0;
    char buf[64] = {0};

    int height = n_options + 3; // 3 - Borders + input line
    int width = max_x - 20;
    int startx = (max_x - width) / 2;
    int starty = max_y / 10;

    WINDOW *palette = newwin(height, width, starty, startx);
    keypad(palette, TRUE);
    wbkgd(palette, COLOR_PAIR(PAIR_BACKGROUND));

    while (1) {
        getmaxyx(stdscr, max_y, max_x);
        width = max_x - 20;
        height = n_options + 3;

        if (width < 1 || height < 1) {
            continue;
        }

        startx = (max_x - width) / 2;
        starty = max_y / 10;
        wresize(palette, height, width);
        mvwin(palette, starty, startx);

        werase(palette);
        box(palette, 0, 0);

        for (int i = 0; i < n_options; i++) {
            if (i == highlight) {
                wattron(palette, COLOR_PAIR(PAIR_STATUS_BAR));
                mvwhline(palette, i + 2, 1, ' ', width - 2);
            }
            mvwprintw(palette, i + 2, 2, "%s", options[i]);
            if (i == highlight) {
                wattroff(palette, COLOR_PAIR(PAIR_STATUS_BAR));
            }

            mvwprintw(palette, 1, 1, "> ");
            mvwprintw(palette, 1, 3, "%s", buf);
        }

        int ch = wgetch(palette);
        if (ch == KEY_UP) {
            highlight = (highlight - 1 + n_options) % n_options;
        } else if (ch == KEY_DOWN) {
            highlight = (highlight + 1) % n_options;
        } else if (ch == '\n') {
            delwin(palette);
            touchwin(stdscr);
            wrefresh(stdscr);
            refresh();
            draw_editor();
            return highlight;
        } else if (ch == 27) {
            delwin(palette);
            touchwin(stdscr);
            wrefresh(stdscr);
            draw_editor();
            return -1;
        } else if (ch > 31 && ch < 127 && pos < (int)sizeof(buf) - 1) {
            buf[pos++] = ch;
            buf[pos] = '\0';
        } else if ((ch == 127 || ch == KEY_BACKSPACE) && pos > 0) {
            buf[--pos] = '\0';
        }

        refresh();
    }
}

char *draw_theme_palette(void) {
    DIR *d;
    struct dirent *dir;

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/.config/tomo/themes", getenv("HOME"));

    d = opendir(path);

    char **themes = NULL;
    int n_themes = 0;

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                char full_path[PATH_MAX];
                sprintf(full_path, "%s/%s", path, dir->d_name);

                themes = realloc(themes, sizeof(char*) * (n_themes + 1));
                themes[n_themes] = strdup(full_path);
                n_themes++;
            }
        }
        closedir(d);
    }

    if (n_themes == 0) {
        editor.bottom_text = "No themes found in ~/.config/tomo/themes";
        return NULL;
    }

    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    int highlight = 0;

    int pos = 0;
    char buf[64] = {0};

    int height = n_themes + 3; // 3 - Borders + input line
    int width = max_x - 20;
    int startx = (max_x - width) / 2;
    int starty = max_y / 10;

    WINDOW *palette = newwin(height, width, starty, startx);
    keypad(palette, TRUE);
    wbkgd(palette, COLOR_PAIR(PAIR_BACKGROUND));

    while (1) {
        getmaxyx(stdscr, max_y, max_x);
        width = max_x - 20;
        height = n_themes + 3;

        if (width < 1 || height < 1) {
            continue;
        }

        startx = (max_x - width) / 2;
        starty = max_y / 10;
        wresize(palette, height, width);
        mvwin(palette, starty, startx);

        werase(palette);
        box(palette, 0, 0);

        for (int i = 0; i < n_themes; i++) {
            if (i == highlight) {
                wattron(palette, COLOR_PAIR(PAIR_STATUS_BAR));
                mvwhline(palette, i + 2, 1, ' ', width - 2);
            }

            char *theme_name = strrchr(themes[i], '/');
            if (theme_name) {
                theme_name++;
            } else {
                theme_name = themes[i];
            }

            mvwprintw(palette, i + 2, 2, "%s", theme_name);
            if (i == highlight) {
                wattroff(palette, COLOR_PAIR(PAIR_STATUS_BAR));
            }

            mvwprintw(palette, 1, 1, "> ");
            mvwprintw(palette, 1, 3, "%s", buf);
        }

        int ch = wgetch(palette);
        if (ch == KEY_UP) {
            highlight = (highlight - 1 + n_themes) % n_themes;
        } else if (ch == KEY_DOWN) {
            highlight = (highlight + 1) % n_themes;
        } else if (ch == '\n') {
            delwin(palette);
            touchwin(stdscr);
            wrefresh(stdscr);
            refresh();
            draw_editor();
            return themes[highlight];

        } else if (ch == 27) {
            delwin(palette);
            touchwin(stdscr);
            wrefresh(stdscr);
            draw_editor();
            return NULL;

        } else if (ch > 31 && ch < 127 && pos < (int)sizeof(buf) - 1) {
            buf[pos++] = ch;
            buf[pos] = '\0';
        } else if ((ch == 127 || ch == KEY_BACKSPACE) && pos > 0) {
            buf[--pos] = '\0';
        }

        refresh();
    }
}
