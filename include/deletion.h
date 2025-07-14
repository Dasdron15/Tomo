#ifndef DELETION_H
#define DELETION_H

#include <stdbool.h>

#include "editor.h"

void normalize_range(Point *a, Point *b);
bool is_at_line_start(void);
void merge_lines(int dst, int src);
bool delete_tab(Point pos);
bool delete_pair(Point pos);
void delete_char(Point pos);
void delete_range(Point start, Point end);

#endif
