#ifndef NEW_LINE_H
#define NEW_LINE_H

#include <stdbool.h>

int calculate_indent(const char* line);
bool is_enclosing_pair(char left, char right);
char *create_right_side(const char *rest_of_line, int indent);

#endif
