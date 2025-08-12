#include <string.h>
#include <stdio.h>

#include "ini_parser.h"

char *strip(char *string, char ch) {
    char *res = string;

    // Left
    while (*res && res[0] == ch) {
        memmove(res, res + 1, strlen(res + 1) + 1);
    }

    // Right
    size_t len = strlen(res);
    while (len > 0 && res[len - 1] == ch) {
        res[--len] = '\0';
    }

    return res;
}

const Values parse_ini_line(const char *filepath, int line) {
    Values values;
    FILE *fp = fopen(filepath, "r");
    

    return values;
}
