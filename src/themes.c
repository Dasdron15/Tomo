#include "themes.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <curses.h>
#include <stdlib.h>

#include "utils.h"

#define NUM_COLORS 21

enum {
    COLOR_DEFAULT,
    COLOR_KEYWORD,
    COLOR_TYPE,
    COLOR_STRING,
    COLOR_NUM,
    COLOR_CHAR,
    COLOR_FUNCTION,
    COLOR_PREPROCESSOR,
    COLOR_COMMENT,
    COLOR_UNACTIVE,
    COLOR_STATUS_BAR,
    COLOR_STATUS_TEXT,
    COLOR_BACKGROUND,
    COLOR_SELECT,
};

static int theme_colors[NUM_COLORS];

static const RGB hex_to_rgb(int hex) {
    RGB color;
    color.r = (hex >> 16) & 0xff;
    color.g = (hex >> 8) & 0xff;
    color.b = hex & 0xff;

    return color;
}

static short rgb_to_ncurses(int val) {
    return (val * 1000) / 255;
}

static void set_theme_color(int idx, int hex) {
    theme_colors[idx] = idx;
    if (can_change_color()) {
        RGB color = hex_to_rgb(hex);
        init_color(idx, rgb_to_ncurses(color.r), rgb_to_ncurses(color.g), rgb_to_ncurses(color.b));
    }
}



bool parse_ini(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;

    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        char *stripped_line = strip(line, ' ');

        if (stripped_line[0] == '\0') continue;
        if (stripped_line[0] == '#' || line[0] == ';') continue;
        if (stripped_line[0] == '[') continue;

        char *eq = strchr(stripped_line, '=');
        if (eq) {
            *eq = '\0';
            char *key = strip(stripped_line, ' ');
            char *value = strip(eq + 1, ' ');

            value = strip(value, '"');
            value = strip(value, '#');
            
            int hex = (int)strtol(value, NULL, 16);

            if (strcmp(key, "default") == 0)
                set_theme_color(COLOR_DEFAULT, hex);
            else if (strcmp(key, "keyword") == 0)
                set_theme_color(COLOR_KEYWORD, hex);
            else if (strcmp(key, "type") == 0)
                set_theme_color(COLOR_TYPE, hex);
            else if (strcmp(key, "string") == 0)
                set_theme_color(COLOR_STRING, hex);
            else if (strcmp(key, "number") == 0)
                set_theme_color(COLOR_NUM, hex);
            else if (strcmp(key, "char") == 0)
                set_theme_color(COLOR_CHAR, hex);
            else if (strcmp(key, "function") == 0)
                set_theme_color(COLOR_FUNCTION, hex);
            else if (strcmp(key, "preprocessor") == 0)
                set_theme_color(COLOR_PREPROCESSOR, hex);
            else if (strcmp(key, "comment") == 0)
                set_theme_color(COLOR_COMMENT, hex);
            else if (strcmp(key, "unactive") == 0)
                set_theme_color(COLOR_UNACTIVE, hex);
            else if (strcmp(key, "status_bar_bg") == 0)
                set_theme_color(COLOR_STATUS_BAR, hex);
            else if (strcmp(key, "status_bar_text") == 0)
                set_theme_color(COLOR_STATUS_TEXT, hex);
            else if (strcmp(key, "background") == 0)
                set_theme_color(COLOR_BACKGROUND, hex);
            else if (strcmp(key, "select") == 0)
                set_theme_color(COLOR_SELECT, hex);
        }
    }

    fclose(fp);
    return true;
}

bool load_theme(char *theme_path) {
    char *theme_name = get_filename(theme_path);
    char *theme_ext = strchr(theme_name, '.');

    if (strcmp(theme_ext, ".ini") != 0) {
        return false;
    } else {
        parse_ini(theme_path);
    }



    return true;
}

// enum {
//     COLOR_DEFAULT,
//     COLOR_KEYWORD,
//     COLOR_TYPE,
//     COLOR_STRING,
//     COLOR_NUM,
//     COLOR_CHAR,
//     COLOR_FUNCTION,
//     COLOR_PREPROCESSOR,
//     COLOR_COMMENT,
//     COLOR_UNACTIVE,
//     COLOR_STATUS_BAR,
//     COLOR_STATUS_TEXT,
//     COLOR_BACKGROUND,
//     COLOR_SELECT,
// };

void init_colors() {
    if (has_colors() == FALSE) {
        endwin();
        printf("Error: terminal does not support colors\n");
        exit(0);
    }

    start_color();

    if (!load_theme("theme.ini")) {
        theme_colors[COLOR_DEFAULT] = COLOR_WHITE;
        theme_colors[COLOR_KEYWORD] = COLOR_CYAN;
        theme_colors[COLOR_TYPE] = COLOR_GREEN;
        theme_colors[COLOR_STRING] = COLOR_YELLOW;
        theme_colors[COLOR_NUM] = COLOR_MAGENTA;
        theme_colors[COLOR_CHAR] = COLOR_YELLOW;
        theme_colors[COLOR_FUNCTION] = COLOR_RED;
        theme_colors[COLOR_PREPROCESSOR] = COLOR_MAGENTA;
        theme_colors[COLOR_COMMENT] = COLOR_GREEN;
        theme_colors[COLOR_UNACTIVE] = COLOR_BLUE;
        theme_colors[COLOR_STATUS_BAR] = COLOR_BLACK;
        theme_colors[COLOR_STATUS_TEXT] = COLOR_WHITE;
        theme_colors[COLOR_BACKGROUND] = -1;
        theme_colors[COLOR_SELECT] = COLOR_BLUE;
    }

    /* Default color pairs */
    init_pair(PAIR_DEFAULT, COLOR_DEFAULT, COLOR_BACKGROUND);
    init_pair(PAIR_KEYWORD, COLOR_KEYWORD, COLOR_BACKGROUND);
    init_pair(PAIR_TYPE, COLOR_TYPE, COLOR_BACKGROUND);
    init_pair(PAIR_STRING, COLOR_STRING, COLOR_BACKGROUND);
    init_pair(PAIR_NUM, COLOR_NUM, COLOR_BACKGROUND);
    init_pair(PAIR_CHAR, COLOR_CHAR, COLOR_BACKGROUND);
    init_pair(PAIR_FUNCTION, COLOR_FUNCTION, COLOR_BACKGROUND);
    init_pair(PAIR_PREPROCESSOR, COLOR_PREPROCESSOR, COLOR_BACKGROUND);
    init_pair(PAIR_UNACTIVE, COLOR_UNACTIVE, COLOR_BACKGROUND);
    init_pair(PAIR_STATUS_BAR, COLOR_DEFAULT, COLOR_STATUS_BAR);
    init_pair(PAIR_BACKGROUND, COLOR_DEFAULT, COLOR_BACKGROUND);
    init_pair(PAIR_COMMENT, COLOR_COMMENT, COLOR_BACKGROUND);

    /* Selection color pairs */
    init_pair(PAIR_SELECT_DEFAULT, COLOR_DEFAULT, COLOR_SELECT);
    init_pair(PAIR_SELECT_KEYWORD, COLOR_KEYWORD, COLOR_SELECT);
    init_pair(PAIR_SELECT_TYPE, COLOR_TYPE, COLOR_SELECT);
    init_pair(PAIR_SELECT_STRING, COLOR_STRING, COLOR_SELECT);
    init_pair(PAIR_SELECT_NUM, COLOR_NUM, COLOR_SELECT);
    init_pair(PAIR_SELECT_CHAR, COLOR_CHAR, COLOR_SELECT);
    init_pair(PAIR_SELECT_FUNCTION, COLOR_FUNCTION, COLOR_SELECT);
    init_pair(PAIR_SELECT_PREPROCESSOR, COLOR_PREPROCESSOR, COLOR_SELECT);
    init_pair(PAIR_SELECT_UNACTIVE, COLOR_UNACTIVE, COLOR_SELECT);
    init_pair(PAIR_SELECT_COMMENT, COLOR_COMMENT, COLOR_SELECT);

    bkgd(COLOR_PAIR(PAIR_BACKGROUND));
}
