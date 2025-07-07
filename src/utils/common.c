#include "common.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

char *mult_char(char c, int count) {
    if (count <= 0) {
        char *empty = malloc(1);
        empty[0] = '\0';
        return empty;
    }

    char *str = malloc(count + 1);
    for (int i = 0; i < count; i++) {
        str[i] = c;
    }
    str[count] = '\0';
    return str;
}

int int_len(int num) { return 1 + (int)log10(num); }

char *get_filename(char *path) {
    char *ptr;
    ptr = path + strlen(path);

    for (; ptr > path; ptr--) {
        if ((*ptr == '\\') || (*ptr == '/')) {
            ptr++;
            break;
        }
    }
    return ptr;
}

int rgb_to_ncurses(int rgb) { return (rgb * 1000) / 255; }

int count_element(const char* string, char ch) {
    int count = 0;

    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == ch) {
            count++;
        }
    }
    return count;
}
