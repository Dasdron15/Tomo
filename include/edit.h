#ifndef EDIT_H
#define EDIT_H

#include "editor.h"

void new_line(void);
void deletion(Point start, Point end);
void insert_char(char c);
void add_tab(void);
void copy_text(Point start, Point end);
void paste_text(void);

#endif
