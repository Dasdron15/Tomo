#ifndef INIT_H
#define INIT_H

#include <stdbool.h>

#define DEFAULT_INDENT_SIZE 4



void init_editor(void);
void init_colors(void);
void init_indent(char *line, bool *indent_measured);

#endif
