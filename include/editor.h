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
    size_t margin;
    
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

void handle_key(int key);
int goto_line(void);
void ask_for_save(void);

void insert_char(char c);
void add_tab();
void deletion(Point start, Point end);
void new_line();

#endif
