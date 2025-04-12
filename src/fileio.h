#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <string.h>

#define MAX_TEXT_SIZE 1000

typedef struct {
    char text[MAX_TEXT_SIZE];
} Editor_State;

void load_file(const char* path, Editor_State* state);
void save_file(const char* path, const Editor_State* state);

#endif