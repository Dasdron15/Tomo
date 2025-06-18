#include "select.h"

static Position selection_start;
static Position selection_end;
static bool selecting = false;

void start_selection(int row, int col) {
    if (!selecting) {
        selection_start.row = row;
        selection_start.col = col;
        selecting = true;
    }
}

void update_selection(int row, int col) {
    if (selecting) {
        selection_end.row = row;
        selection_end.col = col;
    }
}

bool is_selected(int char_y, int char_x) {
    if (selecting && selection_start.row == selection_end.row) {
        return ((selection_start.col >= char_x && selection_end.col <= char_x) || (selection_start.col <= char_x && selection_end.col >= char_x)) && selection_end.row == char_y;
    }
    return false;
}

void cancel_selection() {
    selecting = false;
}
