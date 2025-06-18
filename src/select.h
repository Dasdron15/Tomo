#ifndef SELECT_H
#define SELECT_H

#include <stdbool.h>

typedef struct {
    int row;
    int col;
} Position;

void start_selection(int row, int col);
void update_selection(int row, int col);
bool is_selected(int char_y, int char_x, int cursor_y, int cursor_x);
void cancel_selection();

#endif
