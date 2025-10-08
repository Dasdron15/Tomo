#ifndef UNDO_H
#define UNDO_H

#include <stdbool.h>

void init_undo_redo_stacks(void);
void destroy_undo_redo_stacks(void);
void take_snapshot(bool need_save);
void undo(void);
void redo(void);

#endif
