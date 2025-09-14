#ifndef SELECT_H
#define SELECT_H

#include <stdbool.h>
#include "editor.h"

extern Point selection_start;
extern Point selection_end;

void start_selection(int y, int x);
void update_selection(int y, int x);
void get_selection_bounds(Point* start, Point* end);
bool is_selected(int file_y, int file_x);
bool is_selecting(void);
void cancel_selection(void);
void set_selection(Point start, Point end);
Point get_selection_start(void);
Point get_selection_end(void);

#endif
