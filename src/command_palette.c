#include <curses.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "command_palette.h"
#include "editor.h"
#include "themes.h"

static int draw_palette(char **items, int n_items, char *buf, size_t buf_size) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    int highlight = 0, pos = 0;
    char *filtered[1024];
    int filtered_count = n_items;

    int height = filtered_count + 3;
    int width = max_x - 20;
    int startx = (max_x - width) / 2;
    int starty = max_y / 10;

    WINDOW *palette = newwin(height, width, starty, startx);
    keypad(palette, TRUE);
    wbkgd(palette, COLOR_PAIR(PAIR_BACKGROUND));

    while (1) {
        filtered_count = 0;
        for (int i = 0; i < n_items; i++) {
            if (buf[0] == '\0' || strcasestr(items[i], buf)) {
                filtered[filtered_count++] = items[i];
            }
        }
        if (highlight >= filtered_count) highlight = filtered_count - 1;
        if (highlight < 0) highlight = 0;

        getmaxyx(stdscr, max_y, max_x);
        width = max_x - 20;
        height = filtered_count + 3;
        startx = (max_x - width) / 2;
        starty = max_y / 10;
        wresize(palette, height, width);
        mvwin(palette, starty, startx);

        werase(palette);
        box(palette, 0, 0);

        for (int i = 0; i < filtered_count; i++) {
            if (i == highlight) {
                wattron(palette, COLOR_PAIR(PAIR_STATUS_BAR));
                mvwhline(palette, i + 2, 1, ' ', width - 2);
            }

            char *theme_name = strrchr(filtered[i], '/');
            if (theme_name) {
                theme_name++;
            } else {
                theme_name = filtered[i];
            }

            mvwprintw(palette, i + 2, 2, "%s", theme_name);
            if (i == highlight) wattroff(palette, COLOR_PAIR(PAIR_STATUS_BAR));
        }

        mvwprintw(palette, 1, 1, "> ");
        mvwprintw(palette, 1, 3, "%s", buf);

        int ch = wgetch(palette);
        if (ch == KEY_UP) highlight = (highlight - 1 + filtered_count) % filtered_count;
        else if (ch == KEY_DOWN) highlight = (highlight + 1) % filtered_count;
        else if (ch == '\n') {
            delwin(palette);
            touchwin(stdscr);
            wrefresh(stdscr);
            if (filtered_count > 0) {
                for (int i = 0; i < n_items; i++) {
                    if (items[i] == filtered[highlight]) return i;
                }
            }
            return -1;
        } else if (ch == 27) {
            delwin(palette);
            touchwin(stdscr);
            wrefresh(stdscr);
            return -1;
        }
        else if (ch > 31 && ch < 127 && pos < (int)buf_size - 1) buf[pos++] = ch, buf[pos] = '\0';
        else if ((ch == 127 || ch == KEY_BACKSPACE) && pos > 0) buf[--pos] = '\0';

        draw_editor();
        refresh();
    }
}

int draw_command_palette(void) {
    char *options[] = {"Change theme", "Go to line", "Save file", "Undo", "Exit"};
    char buf[64] = {0};
    return draw_palette(options, sizeof(options) / sizeof(options[0]), buf, sizeof(buf));
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
                snprintf(full_path, PATH_MAX, "%s/%s", path, dir->d_name);

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
    char buf[64] = {0};
    int idx = draw_palette(themes, n_themes, buf, sizeof(buf));
    return idx >= 0 ? themes[idx] : NULL;
}
