#ifndef THEMES_H
#define THEMES_H

#include <stdbool.h>

/* Default color pairs */
#define PAIR_DEFAULT 0
#define PAIR_KEYWORD 1
#define PAIR_TYPE 2
#define PAIR_CUSTOM_TYPE 3
#define PAIR_STRING 4
#define PAIR_NUM 5
#define PAIR_CHAR 6
#define PAIR_FUNCTION 7
#define PAIR_PREPROCESSOR 8
#define PAIR_COMMENT 9
#define PAIR_UNACTIVE 10
#define PAIR_STATUS_BAR 11
#define PAIR_STATUS_TEXT 12
#define PAIR_BACKGROUND 13
#define PAIR_SELECT 14

/* Selection color pairs */
#define PAIR_SELECT_DEFAULT 15
#define PAIR_SELECT_KEYWORD 16
#define PAIR_SELECT_TYPE 17
#define PAIR_SELECT_CUSTOM_TYPE 18
#define PAIR_SELECT_STRING 19
#define PAIR_SELECT_NUM 20
#define PAIR_SELECT_CHAR 21
#define PAIR_SELECT_FUNCTION 22
#define PAIR_SELECT_PREPROCESSOR 23
#define PAIR_SELECT_UNACTIVE 24
#define PAIR_SELECT_COMMENT 25

typedef struct {
    double r;
    double g;
    double b;
} RGB;

void init_colors(void);
void load_theme(const char *filename);

#endif