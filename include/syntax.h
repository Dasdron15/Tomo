#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#include <stddef.h>

extern const char *c_keywords[];

void syntax_init(void);
void syntax_reparse(void);
static int color_for_node_type_c(const char *type);
static int color_for_node_type_py(const char *type);
int get_color_for_pos(int line, int col);

#endif
