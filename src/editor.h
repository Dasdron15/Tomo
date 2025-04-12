#ifndef EDITOR_H
#define EDITOR_H

#include <curses.h>
#include <stdio.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1024

typedef struct {
    char lines[MAX_LINES];
    int cursor_x;
    int cursor_y;
    int num_lines;
} Editor_State;

void init_editor(Editor_State* state);
void draw_editor(Editor_State* state);
void move_cursor(int key, int x, int y, Editor_State* state);
void handle_key(int key, Editor_State* state);

#endif