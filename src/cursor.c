#include "cursor.h"

#include "editor.h"

CursorState cursor;

void move_up(bool is_selecting) {
    if (cursor.y + cursor.y_offset > 0) {
        cursor.y--;
    }
}

void move_down(bool is_selecting) {
    if (cursor.y + cursor.y_offset < editor.total_lines) {
        cursor.y++;
    } else if (cursor.y + cursor.y_offset == editor.total_lines) {
        cursor.x = 
    }
}

void move_right(bool is_selecting) {
    if ()
}

void move_left(bool is_selecting) {

}

void clamp_cursor(void) {
    
}
