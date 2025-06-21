#include "select.h"

static Position selection_start;
static Position selection_end;
static bool selecting = false;

static Position offset_start;

void start_selection(int row, int col, int offset_row, int offset_col) {
    if (!selecting) {
        selection_start.row = row;
        selection_start.col = col;
        selecting = true;

        offset_start.row = offset_row;
        offset_start.col = offset_col;
    }
}

void update_selection(int row, int col, int offset_row, int offset_col) {
    if (selecting) {
        selection_end.row = row;
        selection_end.col = col;

        selection_start.row += (offset_start.row - offset_row);
        selection_start.col += (offset_start.col - offset_col);
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
