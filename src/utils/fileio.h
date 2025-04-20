#ifndef FILEIO_H
#define FILEIO_H

#include "../editor.h"
#include <stdio.h>
#include <string.h>
#include <curses.h>

struct Editor_State;

int load_file(const char* path, struct Editor_State* state);
void save_file(char* path, struct Editor_State* state);

#endif