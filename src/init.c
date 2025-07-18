#include "init.h"

#include "editor.h"

#include <string.h>



void init_editor_color() {
    
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
