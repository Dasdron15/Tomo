#ifndef CURSOR_H
#define CURSOR_H

#include <stddef.h>

typedef struct {
    size_t x;
    size_t y;

    size_t x_offset;
    size_t y_offset;
} CursorState;

extern CursorState cursor;

void move_up(void);
void move_down(void);
void move_right(void);
void move_left(void);

void clamp_cursor(void);

#endif
