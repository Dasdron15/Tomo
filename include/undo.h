#ifndef UNDO_H
#define UNDO_H

#include <stdbool.h>

void init_undo_stack(void);
void take_snapshot(void);
void undo(void);

#endif
