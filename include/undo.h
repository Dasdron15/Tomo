#ifndef UNDO_H
#define UNDO_H

#include <stdbool.h>

void take_snapshot(bool was_previously_deleted);
void undo(void);

#endif
