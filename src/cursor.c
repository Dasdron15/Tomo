#include "cursor.h"

void move_up(void) {
    if (cursor.y + cursor.y_offset > 0) {
        cursor.x--;
    }
}

void move_down(size_t total_lines) {
        
}

void move_right(void) {

}

void move_left(void) {

}

void clamp_cursor(void) {
    
}
