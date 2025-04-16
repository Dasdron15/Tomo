#ifndef EDITOR_H
#define EDITOR_H

#include "utils/common.h"
#include <curses.h>
#include <string.h>


#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024

typedef struct {
    char* lines[MAX_LINES * MAX_LINE_LENGTH]; // Array that stores all the text from a file
    char total_lines;

    int scroll_offset; // Thing that is needed for proper text scrolling
    int max_line_num_len; // Variable that counts how long is the current max line number

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