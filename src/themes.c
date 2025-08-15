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
    COLOR_CUSTOM_TYPE,
    COLOR_STRING,
    COLOR_NUM,
    COLOR_CHAR,
    COLOR_IDENTIFIER,
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

void load_theme(const char *filename) {
    if (filename == NULL) return;

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
            } else if (key_len == strlen("custom_type") && strncmp(src + key_start, "custom_type", key_len) == 0) {
                set_theme_color(COLOR_CUSTOM_TYPE, hex);
            } else if (key_len == strlen("string") && strncmp(src + key_start, "string", key_len) == 0) {
                set_theme_color(COLOR_STRING, hex);
            } else if (key_len == strlen("number") && strncmp(src + key_start, "number", key_len) == 0) {
                set_theme_color(COLOR_NUM, hex);
            } else if (key_len == strlen("char") && strncmp(src + key_start, "char", key_len) == 0) {
                set_theme_color(COLOR_CHAR, hex);
            } else if (key_len == strlen("function") && strncmp(src + key_start, "function", key_len) == 0) {
                set_theme_color(COLOR_FUNCTION, hex);
            } else if (key_len == strlen("identifier") && strncmp(src + key_start, "identifier", key_len) == 0) {
                set_theme_color(COLOR_IDENTIFIER, hex);
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
}

void init_colors(void) {
    if (has_colors() == FALSE) {
        endwin();
        reset();
        fprintf(stderr, "Error: terminal does not support colors\n");
        exit(0);
    }

    start_color();

    // Default theme
    set_theme_color(COLOR_DEFAULT, 0xd0d0d0);
    set_theme_color(COLOR_KEYWORD, 0xff5555);
    set_theme_color(COLOR_TYPE, 0x87cefa);
    set_theme_color(COLOR_CUSTOM_TYPE, 0xffa500);
    set_theme_color(COLOR_STRING, 0x98fb98);
    set_theme_color(COLOR_NUM, 0xf0e68c);
    set_theme_color(COLOR_CHAR, 0x98fb98);
    set_theme_color(COLOR_FUNCTION, 0xdda0dd);
    set_theme_color(COLOR_IDENTIFIER, 0xb0c4de); // Needs an update
    set_theme_color(COLOR_PREPROCESSOR, 0x7fffd4);
    set_theme_color(COLOR_COMMENT, 0x808080);
    set_theme_color(COLOR_UNACTIVE, 0x555555);
    set_theme_color(COLOR_STATUS_BAR, 0x202020);
    set_theme_color(COLOR_STATUS_TEXT, 0xd0d0d0);
    set_theme_color(COLOR_BACKGROUND, 0x121212);
    set_theme_color(COLOR_SELECT, 0x333333);

    /* Default color pairs */
    init_pair(PAIR_DEFAULT, COLOR_DEFAULT, COLOR_BACKGROUND);
    init_pair(PAIR_KEYWORD, COLOR_KEYWORD, COLOR_BACKGROUND);
    init_pair(PAIR_TYPE, COLOR_TYPE, COLOR_BACKGROUND);
    init_pair(PAIR_CUSTOM_TYPE, COLOR_CUSTOM_TYPE, COLOR_BACKGROUND);
    init_pair(PAIR_STRING, COLOR_STRING, COLOR_BACKGROUND);
    init_pair(PAIR_NUM, COLOR_NUM, COLOR_BACKGROUND);
    init_pair(PAIR_CHAR, COLOR_CHAR, COLOR_BACKGROUND);
    init_pair(PAIR_FUNCTION, COLOR_FUNCTION, COLOR_BACKGROUND);
    init_pair(PAIR_IDENTIFIER, COLOR_IDENTIFIER, COLOR_BACKGROUND);
    init_pair(PAIR_PREPROCESSOR, COLOR_PREPROCESSOR, COLOR_BACKGROUND);
    init_pair(PAIR_UNACTIVE, COLOR_UNACTIVE, COLOR_BACKGROUND);
    init_pair(PAIR_STATUS_BAR, COLOR_DEFAULT, COLOR_STATUS_BAR);
    init_pair(PAIR_BACKGROUND, COLOR_DEFAULT, COLOR_BACKGROUND);
    init_pair(PAIR_COMMENT, COLOR_COMMENT, COLOR_BACKGROUND);

    /* Selection color pairs */
    init_pair(PAIR_SELECT_DEFAULT, COLOR_DEFAULT, COLOR_SELECT);
    init_pair(PAIR_SELECT_KEYWORD, COLOR_KEYWORD, COLOR_SELECT);
    init_pair(PAIR_SELECT_TYPE, COLOR_TYPE, COLOR_SELECT);
    init_pair(PAIR_SELECT_CUSTOM_TYPE, COLOR_CUSTOM_TYPE, COLOR_SELECT);
    init_pair(PAIR_SELECT_STRING, COLOR_STRING, COLOR_SELECT);
    init_pair(PAIR_SELECT_NUM, COLOR_NUM, COLOR_SELECT);
    init_pair(PAIR_SELECT_CHAR, COLOR_CHAR, COLOR_SELECT);
    init_pair(PAIR_SELECT_FUNCTION, COLOR_FUNCTION, COLOR_SELECT);
    init_pair(PAIR_SELECT_IDENTIFIER, COLOR_IDENTIFIER, COLOR_SELECT);
    init_pair(PAIR_SELECT_PREPROCESSOR, COLOR_PREPROCESSOR, COLOR_SELECT);
    init_pair(PAIR_SELECT_UNACTIVE, COLOR_UNACTIVE, COLOR_SELECT);
    init_pair(PAIR_SELECT_COMMENT, COLOR_COMMENT, COLOR_SELECT);

    bkgd(COLOR_PAIR(PAIR_BACKGROUND));
}
