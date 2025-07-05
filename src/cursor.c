#include "cursor.h"

#include <curses.h>
#include <string.h>

#include "editor.h"
#include "select.h"

CursorState cursor;

void move_up(bool is_selecting) {
    if (cursor.y + cursor.y_offset > 0) {    
        cursor.y--;
        cursor.x = cursor.max_x;
    } else {
        cursor.x = editor.margin;
        cursor.max_x = cursor.x;
    }
    
    if (!is_selecting) {
        cancel_selection();
    }

    clamp_cursor();
}

void move_down(bool is_selecting) {
    if (cursor.y + cursor.y_offset < editor.total_lines) {
        cursor.y++;
        cursor.x = cursor.max_x;
    } else {
        size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
        cursor.x = line_len;
        cursor.max_x = cursor.x;
    }

    if (!is_selecting) {
        cancel_selection();
    }

    clamp_cursor();
}

void move_right(bool is_selecting) {
    size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);

    if (line_len <= cursor.x - editor.margin &&
        cursor.y + cursor.y_offset < editor.total_lines) {
        cursor.x = editor.margin;
        cursor.y++;
    } else {
        cursor.x++;
    }

    cursor.max_x = cursor.x;

    if (!is_selecting) {
        cancel_selection();
    }

    clamp_cursor();
}

void move_left(bool is_selecting) {
    if (cursor.x <= editor.margin && cursor.y > 0) {
        cursor.y--;
        size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);

        cursor.x = line_len + editor.margin;
    } else if (!(cursor.x <= editor.margin && cursor.y + cursor.y_offset < 1)) {
        cursor.x--;
    }

    cursor.max_x = cursor.x;

    if (!is_selecting) {
        cancel_selection();
    }

    clamp_cursor();
}

void clamp_cursor(void) {
    size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
    
    if (cursor.x + cursor.x_offset > line_len) {
        cursor.x = line_len + editor.margin;
    }
}
