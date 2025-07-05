#ifndef CURSOR_H
#define CURSOR_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    int x;
    int y;
    int max_x;

    size_t x_offset;
    size_t y_offset;
} CursorState;

extern CursorState cursor;

void move_up(bool is_selecting);
void move_down(bool is_selecting);
void move_right(bool is_selecting);
void move_left(bool is_selecting);

void clamp_cursor(void);

#endif
