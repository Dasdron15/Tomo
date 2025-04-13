#include "common.h"

char* mult_char(char c, int count) {
    char* str = malloc(count + 1);
    for (int i = 0; i < count; i++) {
        str[i] = c;
    }
    str[count] = '\0';
    return str;
}