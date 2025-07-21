#include "syntax.h"

#include <ctype.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"
#include "init.h"

/* C KEYWORDSSSSSS!!! (I UFCKING LOVE C) */
static const char *C_keywords[] = {
    "break",  "case",     "auto",   "struct",  "switch", "else",
    "enum",   "continue", "sizeof", "default", "extern", "for",
    "do",     "goto",     "if",     "typedef", "union",  "register",
    "return", "volatile", "while",  NULL};

static const char *C_types[] = {"char",   "short", "double", "int",      "bool",
                                "float",  "void",  "signed", "unsigned", "long",
                                "static", "const", NULL};

/*
char *language = "Python";
short dick = true;
*/

static const char *Py_keywords[] = {
    "def",  "class", "if",     "else",   "elif",  "while",    "for",
    "in",   "try",   "except", "return", "break", "continue", "import",
    "from", "as",    "pass",   "None",   "True",  "False",    NULL};

const SyntaxDef LANGUAGES[] = {{.file_ext = ".c",
                                .keywords = C_keywords,
                                .types = C_types,
                                .sl_comment = "//"},
                               {
                                   .file_ext = ".py",
                                   .keywords = Py_keywords,
                                   .types = NULL,
                                   .sl_comment = "#",
                               },

                               {NULL, NULL, NULL, 0}};

const SyntaxDef *get_syntax() {
    char *ext = strrchr(editor.filename, '.');
    if (!ext)
        return NULL;

    for (int i = 0; LANGUAGES[i].file_ext != NULL; i++) {
        if (strcmp(LANGUAGES[i].file_ext, ext) == 0) {
            return &LANGUAGES[i];
        }
    }

    return NULL;
}

int syntax_color(char *line, int pos, const SyntaxDef *syntax) {
    if (!syntax)
        return COLOR_DEFAULT;

    char c = line[pos];
    int line_len = (int)strlen(line);

    // Highlight comments
    if (syntax->sl_comment) {
        const char *comment = syntax->sl_comment;
        char *dest = strstr(line, comment);
        if (dest != NULL) {
            int comment_start = dest - line;
            if (pos >= comment_start) {
                return PAIR_UNACTIVE;
            }
        }
    }

    if (is_quoted(line, pos)) {
        return PAIR_STRING;
    }
    if (isdigit(c) || (c == '.' && isdigit(line[pos + 1]))) {
        return PAIR_NUM;
    }
    if (syntax->keywords && is_keyword(line, pos, syntax->keywords)) {
        return PAIR_KEYWORD;
    }
    if (syntax->types && is_type(line, pos, syntax->types)) {
        return PAIR_TYPE;
    }
    if (is_function(line, pos)) {
        return PAIR_FUNCTION;
    }

    return PAIR_DEFAULT;
}

bool is_keyword(const char *str, int pos, const char **keywords) {
    int start = pos;
    while (start > 0 && (isalnum(str[start - 1]) || str[start - 1] == '_'))
        start--;

    int end = pos;
    while (isalnum(str[end]) || str[end] == '_')
        end++;

    if (pos >= end)
        return false;

    int len = end - start;
    for (int i = 0; keywords[i]; i++) {
        if (strlen(keywords[i]) == len &&
            strncmp(str + start, keywords[i], len) == 0) {
            return true;
        }
    }

    return false;
}

bool is_type(const char *str, int pos, const char **types) {
    int start = pos;
    while (start > 0 && (isalnum(str[start - 1]) || str[start - 1] == '_'))
        start--;

    int end = pos;
    while (isalnum(str[end]) || str[end] == '_')
        end++;

    if (pos >= end)
        return false;

    int len = end - start;
    for (int i = 0; types[i]; i++) {
        if (strlen(types[i]) == len &&
            strncmp(str + start, types[i], len) == 0) {
            return true;
        }
    }

    return false;
}

bool is_function(const char *str, int pos) {
    int start = pos;
    while (start > 0 && (isalnum(str[start - 1]) || str[start - 1] == '_'))
        start--;

    int end = pos;
    while (isalnum(str[end]) || str[end] == '_')
        end++;

    if (end <= start)
        return false;

    while (str[end] && isspace(str[end]))
        end++;

    if (pos >= end)
        return false;

    if (str[end] != '(')
        return false;

    int paren_depth = 0;
    for (int i = end - 1; str[i]; i++) {
        if (str[i] == ')')
            paren_depth--;
        if (str[i] == '(')
            paren_depth++;
    }
    if (paren_depth == 0)
        return true;

    return false;
}

bool is_quoted(const char *str, int pos) {
    bool in_string = false;
    bool escaped = false;

    for (int i = 0; str[i] && i <= strlen(str); i++) {
        if (str[i] == '\\') {
            escaped = !escaped;
            continue;
        }
        if (str[i] == '"' && !escaped) {
            in_string = !in_string;
        }
        if (i == pos && str[i] == '"') {
            return true;
        }
        if (i == pos) {
            return in_string;
        }

        escaped = false;
    }

    return in_string;
}
