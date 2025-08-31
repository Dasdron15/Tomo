#ifndef UNDO_H
#define UNDO_H

#include <stdbool.h>
#include "editor.h"

#define UNDO_DEPTH 16

typedef struct Snapshot {
    char **lines;
    Point cursor_pos;
    Point selection_start;
    Point selection_end;
} Snapshot;

Snapshot undo_buffer[UNDO_DEPTH];

void take_snapshot(void);
void undo(void);

#endif
