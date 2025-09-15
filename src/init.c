#include "init.h"

#include <string.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#include "editor.h"
#include "utils.h"
#include "cursor.h"

void init_editor(void) {
    editor.bottom_text = "";

    editor.margin = int_len(editor.total_lines) + 2;

    cursor.x = editor.margin;
    cursor.x = editor.margin;
    cursor.max_x = editor.margin;
    cursor.y = 0;
    cursor.x_offset = 0;
    cursor.y_offset = 0;

    printf("\033[5 q"); // Set cursor to line
    initscr();
    raw();
    set_escdelay(0);
    keypad(stdscr, true);
    noecho();
}



void init_indent(char *line, bool *indent_measured) {
    int indent_count = 0;
    editor.tab_width = DEFAULT_INDENT_SIZE;

    if (line[0] == '\t') {
        editor.tab_indent = true;
        editor.indent_size = 1;
        *indent_measured = true;
        return;
    }
    
    if ((line[0] == ' ' && line[1] == ' ') || line[0] == '\t') {
        for (int i = 0; i < (int) strlen(line); i++) {
            if (line[i] == ' ') {
                indent_count++;
            } else {
                break;
            }
        }
        editor.tab_indent = false;
        editor.indent_size = indent_count;
        *indent_measured = true;
        return;
    }
}
