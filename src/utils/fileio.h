#ifndef FILEIO_H
#define FILEIO_H

#include "../editor.h"
#include <stdio.h>
#include <string.h>
#include <curses.h>

int load_file(const char* path, Editor_State* state);
void save_file(char* path, Editor_State* state);

#endif