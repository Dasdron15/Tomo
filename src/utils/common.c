#include "common.h"

char* mult_char(char c, int count) {
    if (count <= 0) {
        char* empty = malloc(1);
        empty[0] = '\0';
        return empty;
    }

    char* str = malloc(count + 1);
    for (int i = 0; i < count; i++) {
        str[i] = c;
    }
    str[count] = '\0';
    return str;
}

int int_len(int num) {
    return 1 + (int)log10(num);
}
