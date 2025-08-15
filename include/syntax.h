#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#include <stddef.h>

void syntax_init(void);
void syntax_reparse(void);
int get_color_for_pos(int line, int col);

#endif
