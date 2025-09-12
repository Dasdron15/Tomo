#ifndef UNDO_H
#define UNDO_H

#include <stdbool.h>

void init_undo_stack(void);
void take_snapshot(bool need_save);
void undo(void);

#endif
