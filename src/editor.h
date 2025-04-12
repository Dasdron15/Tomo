#ifndef EDITOR_H
#define EDITOR_H

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024

typedef struct {
    char text[MAX_LINE_LENGTH * MAX_LINES];
    int text_len;

    int lines_len[MAX_LINES];
    int total_lines;

    int cursor_x;
    int cursor_y;
} Editor_State;

void init_editor(Editor_State* state);
void update_lines(Editor_State* state);
void draw_editor(Editor_State* state);
void move_cursor(int key, int x, int y, Editor_State* state);
void handle_key(int key, Editor_State* state);

#endif