#ifndef INIT_H
#define INIT_H

#include <stdbool.h>

#define DEFAULT_INDENT_SIZE 4

/* Color definitions */
#define COLOR_DEFAULT 8
#define COLOR_KEYWORD 9
#define COLOR_TYPE 10
#define COLOR_STRING 11
#define COLOR_NUMBER 12
#define COLOR_CHAR 13
#define COLOR_FUNCTION 14
#define COLOR_PREPROCESSOR 15
#define COLOR_UNACTIVE 16
#define COLOR_STATUS_BAR 17
#define COLOR_BACKGROUND 18
#define COLOR_SELECT 19

/* Default color pairs */
#define PAIR_DEFAULT 0
#define PAIR_KEYWORD 1
#define PAIR_TYPE 2
#define PAIR_STRING 3
#define PAIR_NUM 4
#define PAIR_CHAR 5
#define PAIR_FUNCTION 6
#define PAIR_PREPROCESSOR 7
#define PAIR_UNACTIVE 8
#define PAIR_STATUS_BAR 9
#define PAIR_BACKGROUND 10
#define PAIR_SELECT 11

/* Selection color pairs */
#define PAIR_SELECT_DEFAULT 12
#define PAIR_SELECT_KEYWORD 13
#define PAIR_SELECT_TYPE 14
#define PAIR_SELECT_STRING 15
#define PAIR_SELECT_NUM 16
#define PAIR_SELECT_CHAR 17
#define PAIR_SELECT_FUNCTION 18
#define PAIR_SELECT_PREPROCESSOR 19
#define PAIR_SELECT_UNACTIVE 20

void init_editor(void);
void init_colors(void);
void init_indent(char *line, bool *indent_measured);

#endif
