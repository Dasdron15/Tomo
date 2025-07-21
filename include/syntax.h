#ifndef SYNTAX_H
#define SYNTAX_H

#include <stddef.h>
#include <stdbool.h>

/* Highlighting for .c files */
extern const char *C_keywords[];
extern const char *C_types[];

int syntax_color(char *line, int pos);
bool is_keyword(const char *str, int pos);
bool is_type(const char *str, int pos);
bool is_function(const char *str, int pos);
bool is_quoted(const char *str, int pos);

#endif
