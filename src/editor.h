#ifndef EDITOR_H
#define EDITOR_H

#include "utils/common.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024

typedef struct {
    char* lines[MAX_LINES * MAX_LINE_LENGTH];
    int scroll_offset;

    int cursor_x;
    int cursor_y;

    int debug_mode;
} Editor_State;

void init_editor(Editor_State* state);
void draw_editor(Editor_State* state);
void move_cursor(int key, Editor_State* state);
void handle_key(int key, Editor_State* state);
void debug_draw(Editor_State* state);

#endif