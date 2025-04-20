#ifndef EDITOR_H
#define EDITOR_H

#include "utils/common.h"
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024

typedef struct {
    char* lines[MAX_LINES * MAX_LINE_LENGTH]; // Array that stores all the text from a file
    char total_lines;

    int scroll_offset; // Thing that is needed for proper text scrolling
    int max_char; // Last symbol X that you were on before pressing up or down arrow

    int cursor_x;
    int cursor_y;

    char* filename;
    bool is_saved;
} Editor_State;

void init_editor(Editor_State* state);
void draw_editor(Editor_State* state);
void move_cursor(int key, Editor_State* state);
void clamp_cursor(Editor_State* state); // Check if cursor is out of bounds
void handle_key(int key, Editor_State* state);
void insert_char(char c, Editor_State* state);
void add_tab(Editor_State* state);
void delete_char(Editor_State* state);
void new_line(Editor_State* state);
void debug_draw(Editor_State* state);

#endif