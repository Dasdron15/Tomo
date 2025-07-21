#ifndef SYNTAX_H
#define SYNTAX_H

#include <stddef.h>

/* Highlighting for .c files */
const char *C_keywords[] = {
    "break",  "case",     "const",    "auto",     "struct",  "switch",
    "else",   "enum",     "continue", "sizeof",   "default", "extern",
    "for",    "do",       "goto",     "if",       "typedef", "union",
    "static", "register", "return",   "volatile", "while",   NULL};
const char *C_types[] = {"char", "short",  "double",   "int",  "float",
                         "void", "signed", "unsigned", "long", NULL};

#endif
