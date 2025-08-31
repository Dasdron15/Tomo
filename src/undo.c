#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curses.h>

#include "undo.h"
#include "editor.h"
#include "select.h"
#include "cursor.h"

void take_snapshot(void) {
    Snapshot snap;
    
    snap.lines = malloc(sizeof(char*) * editor.total_lines);
    
    for (int i = 0; i < editor.total_lines; i++) {
        snap.lines[i] = malloc(strlen(editor.lines[i]) + 1);
        strcpy(snap.lines[i], editor.lines[i]);
    }

    if (is_selecting()) {
        snap.selection_start = get_start();
        snap.selection_end = get_end();
    }

    snap.cursor_pos.x = cursor.x;
    snap.cursor_pos.y = cursor.y;

    undo_buffer[0] = snap;
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
