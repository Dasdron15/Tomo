#include "syntax.h"

#include <ctype.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>

#include "editor.h"

/* C KEYWORDSSSSSS!!! (I UFCKING LOVE C) */
const char *C_keywords[] = {
    "break",  "case",     "const",    "auto",     "struct",  "switch",
    "else",   "enum",     "continue", "sizeof",   "default", "extern",
    "for",    "do",       "goto",     "if",       "typedef", "union",
    "static", "register", "return",   "volatile", "while", NULL};

const char *C_types[] = {
    "char", "short", "double", "int", "float",
    "void", "signed", "unsigned", "long", NULL};


int syntax_color(char *line, int pos) {
    char c = line[pos];
    int line_len = (int) strlen(line);
    int comment_start = -1;

    for (int i = 0; i < line_len - 1; i++) {
        if (line[i] == '/' && line[i + 1] == '/') {
            comment_start = i;
            break;
        }
    }

    if (comment_start != -1 && pos >= comment_start) return 1; // Highlight comments
    if (isdigit(c) || c == '*' || is_quoted(line, pos)) return 5; // Highlight numbers or '*' symbol

    int start = pos;
    while (start > 0 && isalpha(line[start - 1])) start--;
    int end = pos;
    while (isalpha(line[end]) || line[end] == '_') end++;

    if (is_keyword(line, pos)) return 6;
    if (is_type(line, pos)) return 7;
    if (is_function(line, pos)) return 8;

    return 4; // Default
}


bool is_keyword(const char *str, int pos) {
    const char **keywords;
    
    if (strcmp(editor.file_ext, ".c") == 0) {
        keywords = C_keywords;
    } else {
        return false;
    }

    int start = pos;
    while (start > 0 && (isalnum(str[start - 1]) || str[start - 1] == '_')) start--;

    int end = pos;
    while (isalnum(str[end]) || str[end] == '_') end++;

    if (pos >= end) return false;

    int len = end - start;
    for (int i = 0; keywords[i]; i++) {
        if (strlen(keywords[i]) == len && strncmp(str + start, keywords[i], len) == 0) {
            return true;
        }
    }

    return false;
}

bool is_type(const char *str, int pos) {
    const char **types;

    if (strcmp(editor.file_ext, ".c") == 0) {
        types = C_types;
    } else {
        return false;
    }
    
    int start = pos;
    while (start > 0 && (isalnum(str[start - 1]) || str[start - 1] == '_')) start--;

    int end = pos;
    while (isalnum(str[end]) || str[end] == '_') end++;

    if (pos >= end) return false;

    int len = end - start;
    for (int i = 0; types[i]; i++) {
        if (strlen(types[i]) == len && strncmp(str + start, types[i], len) == 0) {
            return true;
        }
    }
    
    return false;
}

bool is_function(const char *str, int pos) {
    int start = pos;
    while (start > 0 && (isalnum(str[start - 1]) || str[start - 1] == '_')) start--;

    int end = pos;
    while (isalnum(str[end]) || str[end] == '_') end++;

    if (end <= start) return false;

    while (str[end] && isspace(str[end])) end++;

    if (pos >= end) return false;

    if (str[end] != '(') return false;

    int paren_depth = 0;
    for (int i = end - 1; str[i]; i++) {    
        if (str[i] == ')') paren_depth--;
        if (str[i] == '(') paren_depth++;
    }
    if (paren_depth == 0) return true;
    
    return false;
}

bool is_quoted(const char *str, int pos) {
    bool in_string = false;

    for (int i = 0; str[i] && i <= pos; i++) {
        if (str[i] == '"' && (i == 0 || str[i - 1] != '\\')) {
            in_string = !in_string;
        }
    }
    
    return in_string;
}

