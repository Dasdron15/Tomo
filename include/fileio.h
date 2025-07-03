#ifndef FILEIO_H
#define FILEIO_H

struct Editor_State;

void load_file(const char *path, struct Editor_State *state);
void save_file(struct Editor_State *state);
void save_pos(struct Editor_State *state);
int load_pos(struct Editor_State *state);

#endif
