#ifndef SELECT_H
#define SELECT_H

#include <stdbool.h>
#include "editor.h"

void start_selection(int y, int x);
void update_selection(int y, int x);
void get_selection_bounds(Point* start, Point* end);
bool is_selected(int file_y, int file_x);
bool is_selecting();
void cancel_selection();
Point get_start();

#endif
