#ifndef EDITOR_H
#define EDITOR_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024
#define TAB_SIZE 4

typedef struct {
    char *lines[MAX_LINES *
                MAX_LINE_LENGTH]; // Array that stores all the text from a file
    size_t total_lines;

    int x_offset; // Thing that is needed for proper text scrolling
    int y_offset;

    int max_char; // Last symbol X that you were on before pressing up or down
                  // arrow

    int cursor_x;
    int cursor_y;

    char *filename;
    bool is_saved;
} EditorState;

extern EditorState editor;

typedef struct {
    int y;
    int x;
} Point;

void init_editor(void);
void draw_editor(void);

void move_cursor(int key, bool is_selecting);

void handle_key(int key);
int goto_line(void);
void ask_for_save(void);

void insert_char(char c);
void add_tab();
void deletion(Point start, Point end);
void new_line();

#endif
