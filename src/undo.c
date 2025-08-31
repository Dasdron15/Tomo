#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curses.h>

#include "undo.h"
#include "editor.h"
#include "select.h"
#include "cursor.h"

#define UNDO_DEPTH 16

static struct Snapshot {
    char **lines;
    Point cursor_pos;
    Point selection_start;
    Point selection_end;
} Snapshot;

static struct Snapshot undo_buffer[UNDO_DEPTH];

/*
take_snapshot function copies the whole file into Snapshot buffer and puts
the struct to the first position of undo_buffer
*/
void take_snapshot(bool merge_with_previous) {
    struct Snapshot snapshot;
    
    snapshot.lines = malloc(sizeof(char*) * editor.total_lines);
    
    for (int i = 0; i < editor.total_lines; i++) {
        snapshot.lines[i] = malloc(strlen(editor.lines[i]) + 1);
        strcpy(snapshot.lines[i], editor.lines[i]);
    }

    if (is_selecting()) {
        snapshot.selection_start = get_start();
        snapshot.selection_end = get_end();
    }

    snapshot.cursor_pos.x = cursor.x;
    snapshot.cursor_pos.y = cursor.y;

    if (!merge_with_previous) {
        undo_buffer[0] = snapshot;
    }
}

void undo(void) {
    for (int i = 0; i < editor.total_lines; i++) {
        editor.lines[i] = realloc(editor.lines[i], strlen(undo_buffer[0].lines[i]) + 1);
        editor.lines[i] = undo_buffer[0].lines[i];
    }

    if (is_selecting()) {
        selection_start = undo_buffer[0].selection_start;
        selection_end = undo_buffer[0].selection_end;
    }

    cursor.x = undo_buffer[0].cursor_pos.x;
    cursor.y = undo_buffer[0].cursor_pos.y;
}
