#ifndef FILEIO_H
#define FILEIO_H

#include <stdbool.h>

void load_file(const char *path);
bool save_file(void);
void open_dir(char *path);

#endif
