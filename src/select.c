#include "select.h"

static Position selection_start;
static Position selection_end;
static bool selecting = false;

void start_selection(int row, int col) {
    if (!selecting) {
        selection_start.y = row;
        selection_start.x = col;
        selecting = true;
    }
}

void update_selection(int row, int col) {
    if (selecting) {
        selection_end.y = row;
        selection_end.x = col;
    }
}

bool is_selected(int file_y, int file_x) {
    Position start;
    Position end;

    if (!selecting)
        return false;

    if (selection_start.y > selection_end.y ||
        (selection_start.x > selection_end.x &&
         selection_start.y == selection_end.y)) {
        start = selection_end;
        end = selection_start;
    } else {
        start = selection_start;
        end = selection_end;
    }

    if (file_y < start.y || file_y > end.y)
        return false;

    if (start.y == end.y) {
        return file_x >= start.x && file_x < end.x;
    }

    if (file_y == start.y) {
        return file_x >= start.x;
    }

    if (file_y == end.y) {
        return file_x < end.x;
    }

    return true;
}

bool is_selecting() { return selecting; }

void cancel_selection() { selecting = false; }

Position get_start() { return selection_start; }
