#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include "editor.h"

void load_file(const char* path, Editor_State* state);
void save_file(const char* path, const Editor_State* state);

#endif