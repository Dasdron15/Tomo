#include "cursor.h"

#include <curses.h>
#include <string.h>

#include "editor.h"
#include "select.h"

CursorState cursor;

void move_up(bool is_selecting) {
    cursor.y--;

    if (!is_selecting) {
        cancel_selection();
    }
}

void move_down(bool is_selecting) {
    cursor.y++;

    if (!is_selecting) {
        cancel_selection();
    }
}

void move_right(bool is_selecting) {
    size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
    
    if (line_len <= cursor.x - editor.margin && cursor.y + cursor.y_offset < editor.total_lines) {
        cursor.x = editor.margin;
        cursor.y++;
    } else {
        cursor.x++;
    }

    if (!is_selecting) {
        cancel_selection();
    }
}

void move_left(bool is_selecting) {
    if (cursor.x <= editor.margin && cursor.y > 0) {
        cursor.y--;
        
        size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
        cursor.x = line_len + editor.margin;
    } else if (!(cursor.x <= editor.margin && cursor.y + cursor.y_offset < 1)) {
        cursor.x--;
    }

    if (!is_selecting) {
        cancel_selection();
    }
}

void clamp_cursor(void) {
    if (true) {
        
    }
}
