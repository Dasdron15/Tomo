#include "file_tree.h"

#include <curses.h>
#include <stdio.h>

void draw_tree(char **files, int el_num) {
    for (int i = 0; i < el_num; i++) {
        printf("%s\n", files[i]);
    }
}
