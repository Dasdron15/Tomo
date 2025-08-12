#ifndef THEMES_H
#define THEMES_H

#include <stdbool.h>

/* Default color pairs */
#define PAIR_DEFAULT 0
#define PAIR_KEYWORD 1
#define PAIR_TYPE 2
#define PAIR_STRING 3
#define PAIR_NUM 4
#define PAIR_CHAR 5
#define PAIR_FUNCTION 6
#define PAIR_PREPROCESSOR 7
#define PAIR_COMMENT 8
#define PAIR_UNACTIVE 9
#define PAIR_STATUS_BAR 10
#define PAIR_STATUS_TEXT 11
#define PAIR_BACKGROUND 12
#define PAIR_SELECT 13

/* Selection color pairs */
#define PAIR_SELECT_DEFAULT 13
#define PAIR_SELECT_KEYWORD 14
#define PAIR_SELECT_TYPE 15
#define PAIR_SELECT_STRING 16
#define PAIR_SELECT_NUM 17
#define PAIR_SELECT_CHAR 18
#define PAIR_SELECT_FUNCTION 19
#define PAIR_SELECT_PREPROCESSOR 20
#define PAIR_SELECT_UNACTIVE 21
#define PAIR_SELECT_COMMENT 22

typedef struct {
    double r;
    double g;
    double b;
} RGB;

typedef struct {
    char key[256];
    char value[256];
} Values;

void init_colors(void);

#endif