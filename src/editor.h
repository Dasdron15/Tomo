#ifndef EDITOR_H
#define EDITOR_H

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024
#define TAB_SIZE 4

struct Editor_State {
    char* lines[MAX_LINES * MAX_LINE_LENGTH]; // Array that stores all the text from a file
    int total_lines;

    int x_offset; // Thing that is needed for proper text scrolling
    int y_offset;
    
    int max_char; // Last symbol X that you were on before pressing up or down arrow

    int cursor_x;
    int cursor_y;

    char* filename;
    bool is_saved;
};

typedef struct {
    int row;
    int col;
} Point;

void init_editor(struct Editor_State* state);
void draw_editor(struct Editor_State* state);

void move_cursor(int key, struct Editor_State* state, bool is_selecting);

void handle_key(int key, struct Editor_State* state);
int goto_line(struct Editor_State* state);
void ask_for_save(struct Editor_State* state);

void insert_char(char c, struct Editor_State* state);
void add_tab(struct Editor_State* state);
void deletion(struct Editor_State* state, Point from, Point to);
void new_line(struct Editor_State* state);

#endif
