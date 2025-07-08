#ifndef EDITOR_H
#define EDITOR_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024
#define TAB_SIZE 4

typedef struct {
    char *lines[MAX_LINES * MAX_LINE_LENGTH]; // MAX_LINES * MAX_LINE_LENGTH
    int total_lines;
    int margin;
    
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
void ask_for_save(void);

#endif
