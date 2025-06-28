#ifndef FILEIO_H
#define FILEIO_H

#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct Editor_State;

void load_file(const char *path, struct Editor_State *state);
void save_file(struct Editor_State *state);
void save_pos(struct Editor_State *state);
int load_pos(struct Editor_State *state);

#endif
