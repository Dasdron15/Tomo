#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#include <stddef.h>

enum SyntaxColors {
    COLOR_DEFAULT = 1,
    COLOR_KEYWORD = 2,
    COLOR_TYPE = 3,
    COLOR_STRING = 4,
    COLOR_NUMBER = 5,
    COLOR_CHAR = 6,
    COLOR_COMMENT = 7,
    COLOR_FUNCTION = 8,
    COLOR_PREPROCESSOR = 9,
};

typedef struct {
    const char *file_ext;
    const char **keywords;
    const char **types;
    const char *sl_comment; // Single line comment
} SyntaxDef;

const SyntaxDef *get_syntax();
int syntax_color(char *line, int pos, const SyntaxDef *syntax);
bool is_keyword(const char *str, int pos, const char **keywords);
bool is_type(const char *str, int pos, const char **types);
bool is_function(const char *str, int pos);
bool is_quoted(const char *str, int pos);

#endif
