#include "themes.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <curses.h>
#include <stdlib.h>

#include "utils.h"
#include "editor.h"

static const RGB hex_to_rgb(int hex) {
    RGB color;
    color.r = (hex >> 16) & 0xff;
    color.g = (hex >> 8) & 0xff;
    color.b = hex & 0xff;

    return color;
}

static int parse_ini(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 1;

    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        char *stripped_line = strip(line, ' ');

        if (stripped_line[0] == '\0') continue;
        if (stripped_line[0] == '#' || line[0] == ';') continue;
        if (stripped_line[0] == '[') continue;

        char *eq = strchr(stripped_line, '=');
        if (eq) {
            *eq = '\0';
            char *key = stripped_line;
            char *value = eq + 1;

            key = strip(key, ' ');
            value = strip(value, ' ');

            endwin();
            reset();
            printf("%s = %s", key, value);
            exit(0);
        }
    }

    fclose(fp);
    return 0;
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
