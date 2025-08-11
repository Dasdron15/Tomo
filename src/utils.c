#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

int count_char(char *string, char ch) {
    int count = 0;

    for (int i = 0; i < (int)strlen(string); i++) {
        if (string[i] == ch) {
            count++;
        }
    }
    return count;
}

char **split(char *str, char delim) {
    int capacity = 10;
    int count = 0;
    char **tokens = malloc(capacity * sizeof(char *));

    char *start = str;
    char *ptr = str;

    while (*ptr) {
        if (*ptr == delim) {
            int len = ptr - start;
            char *token = malloc(len + 1);
            strncpy(token, start, len);
            token[len] = '\0';

            if (count >= capacity) {
                capacity *= 2;
                tokens = realloc(tokens, capacity * sizeof(char *));
            }
            tokens[count++] = token;

            start = ptr + 1;
        }
        ptr++;
    }

    char *token = malloc(ptr - start + 1);
    strcpy(token, start);
    tokens[count++] = token;

    return tokens;
}

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
