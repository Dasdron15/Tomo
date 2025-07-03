#ifndef SELECT_H
#define SELECT_H

#include <stdbool.h>

typedef struct {
    int y;
    int x;
} Position;

void start_selection(int row, int col);
void update_selection(int row, int col);
bool is_selected(int file_y, int file_x);
bool is_selecting();
void cancel_selection();
Position get_start();

#endif
