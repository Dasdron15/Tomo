#ifndef EDITOR_H
#define EDITOR_H

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024

struct Editor_State {
    char* lines[MAX_LINES * MAX_LINE_LENGTH]; // Array that stores all the text from a file
    unsigned int total_lines;

    unsigned int scroll_offset; // Thing that is needed for proper text scrolling
    unsigned int max_char; // Last symbol X that you were on before pressing up or down arrow

    unsigned int cursor_x;
    unsigned int cursor_y;

    char* filename;
    bool is_saved;
};

void init_editor(struct Editor_State* state);
void draw_editor(struct Editor_State* state);
void move_cursor(int key, struct Editor_State* state);
void clamp_cursor(struct Editor_State* state); // Check if cursor is out of bounds
void handle_key(int key, struct Editor_State* state);
void insert_char(char c, struct Editor_State* state);
void add_tab(struct Editor_State* state);
void delete_char(struct Editor_State* state);
void new_line(struct Editor_State* state);

#endif