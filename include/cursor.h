#ifndef CURSOR_H
#define CURSOR_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    size_t x;
    size_t y;
    size_t max_x;

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
