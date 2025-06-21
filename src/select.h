#ifndef SELECT_H
#define SELECT_H

#include <stdbool.h>

typedef struct {
    int row;
    int col;
} Position;

void start_selection(int row, int col, int y_offset, int x_offset);
void update_selection(int row, int col, int y_offset, int x_offset);
bool is_selected(int char_y, int char_x, int cursor_y);
void cancel_selection();

#endif
