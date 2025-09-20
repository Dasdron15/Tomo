#ifndef THEMES_H
#define THEMES_H

#include <stdbool.h>

/* Colors */
#define COLOR_DEFAULT 16
#define COLOR_KEYWORD 17
#define COLOR_TYPE 18
#define COLOR_CUSTOM_TYPE 19
#define COLOR_STRING 20
#define COLOR_NUM 21
#define COLOR_CHAR 22
#define COLOR_IDENTIFIER 23
#define COLOR_FUNCTION 24
#define COLOR_PREPROCESSOR 25
#define COLOR_COMMENT 26
#define COLOR_UNACTIVE 27
#define COLOR_STATUS_BAR 28
#define COLOR_STATUS_TEXT 29
#define COLOR_BACKGROUND 30
#define COLOR_SELECT 31

/* Default color pairs */
#define PAIR_DEFAULT 0
#define PAIR_KEYWORD 1
#define PAIR_TYPE 2
#define PAIR_CUSTOM_TYPE 3
#define PAIR_STRING 4
#define PAIR_NUM 5
#define PAIR_CHAR 6
#define PAIR_FUNCTION 7
#define PAIR_IDENTIFIER 8
#define PAIR_PREPROCESSOR 9
#define PAIR_COMMENT 10
#define PAIR_UNACTIVE 11
#define PAIR_STATUS_BAR 12
#define PAIR_STATUS_TEXT 13
#define PAIR_BACKGROUND 14
#define PAIR_SELECT 15

/* Selection color pairs */
#define PAIR_SELECT_DEFAULT 16
#define PAIR_SELECT_KEYWORD 17
#define PAIR_SELECT_TYPE 18
#define PAIR_SELECT_CUSTOM_TYPE 19
#define PAIR_SELECT_STRING 20
#define PAIR_SELECT_NUM 21
#define PAIR_SELECT_CHAR 22
#define PAIR_SELECT_FUNCTION 23
#define PAIR_SELECT_IDENTIFIER 24
#define PAIR_SELECT_PREPROCESSOR 25
#define PAIR_SELECT_UNACTIVE 26
#define PAIR_SELECT_COMMENT 27

typedef struct {
    double r;
    double g;
    double b;
} RGB;

void init_colors(void);
void load_theme(const char *filename);

#endif