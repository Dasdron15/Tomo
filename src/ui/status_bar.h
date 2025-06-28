#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <curses.h>
#include <stdio.h>

struct Editor_State;

void draw_status_bar(struct Editor_State *state);

#endif
