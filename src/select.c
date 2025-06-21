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

bool is_selected(int file_y, int file_x) {
    if (!selecting) return false;

    Position start = selection_start;
    Position end = selection_end;

    if (file_y < start.row || file_y > end.row) return false;

    if (start.row == end.row) {
        return file_x >= start.col && file_x < end.col;
    }

    if (file_y == start.row) {
        return file_x >= start.col;
    }

    if (file_y == end.row) {
        return file_x < end.col;
    }

    return true;
}

void cancel_selection() {
    selecting = false;
}
