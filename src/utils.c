#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

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

int int_len(int num) {
    int count = 0;
    
    while (num > 0) {
        num = num / 10;
        count++;
    }

    return count;
}

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

char *file_to_string(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;
    
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, size, fp);
    if (bytes_read != size) {
        free(buffer);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    buffer[size] = '\0';
    return buffer;
}
