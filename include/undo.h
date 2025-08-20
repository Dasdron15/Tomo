#ifndef UNDO_H
#define UNDO_H

#include <stdbool.h>
#include "editor.h"

#define UNDO_DEPTH 16

typedef struct Snapshot {
    char **lines;
    bool was_selected;
    Point selection_start;
    Point selection_end;
} Snapshot;

Snapshot shapshots[UNDO_DEPTH];

void take_snapshot(void);
void undo(void);

#endif
