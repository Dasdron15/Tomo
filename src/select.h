#ifndef SELECT_H
#define SELECT_H

#include <stdbool.h>

typedef struct {
    int row;
    int col;
} Position;

void start_selection(int row, int col);
void update_selection(int row, int col);

#endif
