#include "select.h"

#include "cursor.h"
#include "editor.h"

static bool selecting = false;
Point selection_start;
Point selection_end;

void start_selection(int y, int x) {
    if (!selecting) {
        selection_start.y = y;
        selection_start.x = x;
        selecting = true;
    }
}

void update_selection(int y, int x) {
    if (selecting) {
        selection_end.y = y;
        selection_end.x = x;
    }
}

/*
 * Calculates the start and end points of the current text selection.
 * If no selection is active, both points are set to the current cursor position.
 * Otherwise determines the correct order of start and end depending on
 * cursor and selection_start coordinates.
 */

void get_selection_bounds(Point *start, Point *end) {
    int x_pos = cursor.x + cursor.x_offset - editor.margin;
    int y_pos = cursor.y + cursor.y_offset;

    if (!is_selecting()) {
        start->x = x_pos - 1;
        start->y = y_pos;

        end->x = x_pos - 1;
        end->y = y_pos;
    } else if ((selection_start.x > x_pos && selection_start.y == y_pos) ||
                (selection_start.y > y_pos)) {
        start->x = x_pos;
        start->y = y_pos;
        *end = selection_start;
    } else {
        *start = selection_start;
        end->x = x_pos;
        end->y = y_pos;
    }
}

/*
 * Returns true if the given file coordinates are within the current selection range.
 * Otherwise, returns false.
 */
bool is_selected(int file_y, int file_x) {
    Point start;
    Point end;

    if (!selecting)
        return false;

    // Ensure start is always before end in visual order
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

bool is_selecting(void) {
    return selecting;
}

void cancel_selection(void) {
    selecting = false;
}

void set_selection(Point start, Point end) {
    selection_start = start;
    selection_end = end;
    selecting = true;
}

Point get_selection_start(void) {
    return selection_start;
}

Point get_selection_end(void) {
    return selection_end;
}
