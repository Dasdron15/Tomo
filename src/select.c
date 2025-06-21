#include "select.h"

static Position selection_start;
static Position selection_end;
static bool selecting = false;

void start_selection(int row, int col, int y_offset, int x_offset) {
    if (!selecting) {
        selection_start.row = row + y_offset;
        selection_start.col = col + x_offset;
        selecting = true;
    }
}

void update_selection(int row, int col, int y_offset, int x_offset) {
    if (selecting) {
        selection_end.row = row + y_offset;
        selection_end.col = col + x_offset;
    }
}

bool is_selected(int char_y, int char_x, int cursor_y) {
    if (selecting) {
        if (selection_start.row == cursor_y && selection_end.row == char_y) {
            return (selection_start.col >= char_x && selection_end.col <= char_x) || (selection_start.col <= char_x && selection_end.col >= char_x);
        }
        else if (selection_start.row > cursor_y) { // Up arrow
            return (selection_start.row == char_y && selection_start.col >= char_x) || (selection_start.row > char_y && selection_end.row < char_y) || (selection_end.row == char_y && selection_end.col <= char_x);
        }
        else if (selection_start.row < cursor_y) {
            return (selection_start.row == char_y && selection_start.col <= char_x) || (selection_start.row < char_y && selection_end.row > char_y) || (selection_end.row == char_y && selection_end.col >= char_x);
        }
    }

    return false;
}

void cancel_selection() {
    selecting = false;
}
