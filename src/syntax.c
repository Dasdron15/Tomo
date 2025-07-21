#include "syntax.h"

#include <ctype.h>
#include <string.h>

#include "editor.h"

const char *C_keywords[] = {
    "break",  "case",     "const",    "auto",     "struct",  "switch",
    "else",   "enum",     "continue", "sizeof",   "default", "extern",
    "for",    "do",       "goto",     "if",       "typedef", "union",
    "static", "register", "return",   "volatile", "while",   NULL};

const char *C_types[] = {
    "char", "short", "double", "int", "float",
    "void", "signed", "unsigned", "long", NULL};

int is_separator(char c) {
    return isspace(c) || c == '\0' || strchr("(){}[];,+-/*%=!<>&|^~", c);
}

bool is_keyword(const char *str, int len) {
    const char **keywords;
    
    if (strcmp(editor.file_ext, "c") == 0) {
        keywords = C_keywords;
    }

    for (int i = 0; keywords[i]; i++) {
        if ((int) strlen(keywords[i]) == len && strncmp(str, keywords[i], len) == 0)
            return true;
    }
    return false;
}

bool is_type(const char *str, int len) {
    const char **types;

    if (strcmp(editor.file_ext, "c") == 0) {
        types = C_types;
    }

    for (int i = 0; types[i]; i++) {
        if ((int) strlen(types[i]) == len && strncmp(str, types[i], len) == 0)
            return true;
    }
    return false;
}
