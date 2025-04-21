#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <curses.h>

struct Editor_State;

void load_file(const char* path, struct Editor_State* state);
void save_file(struct Editor_State* state);

#endif