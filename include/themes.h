#ifndef THEMES_H
#define THEMES_H

#include <stdbool.h>

typedef struct {
    double r;
    double g;
    double b;
} RGB;

bool load_theme(char *theme_path);

#endif