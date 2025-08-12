#ifndef INI_PARSER_H
#define INI_PARSER_H

typedef struct {
    char section[64];
    char key[64];
    char value[64];
} Values;

char *strip(char *string, char ch);
const Values parse_ini_line(const char *filepath, int line);

#endif