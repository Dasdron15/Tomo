#ifndef EDITOR_H
#define EDITOR_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024

typedef struct {
    char **lines;
    int total_lines;
    int capacity;
    int margin;
    int indent_size;
    int tab_width;
    bool tab_indent;
    char *filename;
    char *bottom_text;
} EditorState;

extern EditorState editor;

typedef struct {
    int y;
    int x;
} Point;

void draw_editor(void);
void handle_key(int key);
void ask_for_save(void);
bool is_saved(void);
void exit_editor(void);
void reset(void);

#endif
