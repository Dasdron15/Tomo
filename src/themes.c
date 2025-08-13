#include "themes.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <curses.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <ctype.h>

#include "utils.h"
#include "editor.h"
#include "tree_sitter/api.h"

#define NUM_COLORS 21

extern const TSLanguage *tree_sitter_ini();

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

static bool load_theme(const char *filename) {
    char *src = file_to_string(filename);

    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_ini());
    TSTree *tree = ts_parser_parse_string(parser, NULL, src, strlen(src));

    TSNode root = ts_tree_root_node(tree);
    uint32_t count = ts_node_child_count(root);

    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(root, i);

        if (strcmp(ts_node_type(child), "setting") == 0) {
            TSNode key_node = ts_node_child(child, 0);
            TSNode value_node = ts_node_child(child, 2);

            uint32_t key_start = ts_node_start_byte(key_node);
            uint32_t key_end = ts_node_end_byte(key_node);
            uint32_t value_start = ts_node_start_byte(value_node);
            uint32_t value_end = ts_node_end_byte(value_node);
            
            // Strip value spaces
            while (value_start < value_end && isspace((unsigned char)src[value_start])) {
                value_start++;
            }
            while (value_end > value_start && isspace((unsigned char)src[value_end - 1])) {
                value_end--;
            }

            // Strip quotes
            if (src[value_start] == '"' && src[value_end - 1] == '"' && value_end - value_start >= 2) {
                value_start++;
                value_end--;
            }

            if (src[value_start] == '#') {
                value_start++;
            }

            size_t value_len = value_end - value_start;
            size_t key_len = key_end - key_start;

            char buf[64];
            sprintf(buf, "%.*s", value_end - value_start, src + value_start);

            int hex = (int)strtol(buf, NULL, 16);

            if (key_len == strlen("default") && strncmp(src + key_start, "default", key_len) == 0) {
                set_theme_color(COLOR_DEFAULT, hex);
            } else if (key_len == strlen("keyword") && strncmp(src + key_start, "keyword", key_len) == 0) {
                set_theme_color(COLOR_KEYWORD, hex);
            } else if (key_len == strlen("type") && strncmp(src + key_start, "type", key_len) == 0) {
                set_theme_color(COLOR_TYPE, hex);
            } else if (key_len == strlen("string") && strncmp(src + key_start, "string", key_len) == 0) {
                set_theme_color(COLOR_STRING, hex);
            } else if (key_len == strlen("number") && strncmp(src + key_start, "number", key_len) == 0) {
                set_theme_color(COLOR_NUM, hex);
            } else if (key_len == strlen("char") && strncmp(src + key_start, "char", key_len) == 0) {
                set_theme_color(COLOR_CHAR, hex);
            } else if (key_len == strlen("function") && strncmp(src + key_start, "function", key_len) == 0) {
                set_theme_color(COLOR_FUNCTION, hex);
            } else if (key_len == strlen("preprocessor") && strncmp(src + key_start, "preprocessor", key_len) == 0) {
                set_theme_color(COLOR_PREPROCESSOR, hex);
            } else if (key_len == strlen("comment") && strncmp(src + key_start, "comment", key_len) == 0) {
                set_theme_color(COLOR_COMMENT, hex);
            } else if (key_len == strlen("unactive") && strncmp(src + key_start, "unactive", key_len) == 0) {
                set_theme_color(COLOR_UNACTIVE, hex);
            } else if (key_len == strlen("status_bar_bg") && strncmp(src + key_start, "status_bar_bg", key_len) == 0) {
                set_theme_color(COLOR_STATUS_BAR, hex);
            } else if (key_len == strlen("status_bar_text") && strncmp(src + key_start, "status_bar_text", key_len) == 0) {
                set_theme_color(COLOR_STATUS_TEXT, hex);
            } else if (key_len == strlen("background") && strncmp(src + key_start, "background", key_len) == 0) {
                set_theme_color(COLOR_BACKGROUND, hex);
            } else if (key_len == strlen("select") && strncmp(src + key_start, "select", key_len) == 0) {
                set_theme_color(COLOR_SELECT, hex);
            }
        }
    }

    ts_tree_delete(tree);
    ts_parser_delete(parser);
    free(src);

    return true;
}

void init_colors(void) {
    if (has_colors() == FALSE) {
        endwin();
        printf("Error: terminal does not support colors\n");
        exit(0);
    }

    start_color();

    DIR *d;
    struct dirent *dir;

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/.config/tomo/themes", getenv("HOME"));

    d = opendir(path);

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                sprintf(path, "%s/%s", path, dir->d_name);

                if (!load_theme(path)) {
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
            }
        }
        closedir(d);
    } else {


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
