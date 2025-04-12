#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>

#define MAX_TEXT_SIZE 1024

typedef struct {
    char Text[MAX_TEXT_SIZE];
} Text_Buffer;

void load_file(const char* path, Text_Buffer* buffer);
void save_file(const char* path, const Text_Buffer* buffer);

#endif