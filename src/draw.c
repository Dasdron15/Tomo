#include "draw.h"

#include "editor.h"

int calc_render_x(const char *line, int cursor_x) {
    int render_x = 0;
    for (int i = 0; i < cursor_x; i++) {
        if (line[i] == '\t') {
            render_x += editor.indent_size;
        } else {
            render_x++;
        }
    }
    return render_x;
}

void draw_line(int row) {
    
}
