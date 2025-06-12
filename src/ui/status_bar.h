#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <stdio.h>
#include <curses.h>

struct Editor_State;

void draw_status_bar(struct Editor_State* state);

#endif
