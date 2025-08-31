#ifndef UNDO_H
#define UNDO_H

#include <stdbool.h>

void take_snapshot(bool merge_with_previous);
void undo(void);

#endif
