#include "syntax.h"

#include <assert.h>
#include <curses.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "editor.h"
#include "init.h"
#include "tree_sitter/api.h"

/* I fucking hate making this stupid syntax highlighting */

const char *c_keywords[] = {
    "if",   "else",    "while",    "for",      "return", "switch",
    "case", "break",   "continue", "struct",   "enum",   "union",
    "goto", "sizeof",  "typedef",  "const",    "static", "extern",
    "do",   "default", "signed",   "unsigned", NULL};

const char *py_keywords[] = {
    "False", "True", NULL
};

const TSLanguage *tree_sitter_c();
const TSLanguage *tree_sitter_python();

static TSParser *parser = NULL;
static TSTree *tree = NULL;
static const char *file_ext = NULL;

const TSLanguage *get_language(const char *file_ext) {
    if (file_ext == NULL) return NULL;

    if (strcmp(file_ext, ".c") == 0) {
        return tree_sitter_c();
    } else if (strcmp(file_ext, ".py") == 0) {
        return tree_sitter_python();
    }

    return NULL;
}

void syntax_init(void) {
    parser = ts_parser_new();
    file_ext = strchr(editor.filename, '.');
    ts_parser_set_language(parser, get_language(file_ext));
}

void syntax_reparse(void) {
    size_t len = 0;
    for (int i = 0; i < editor.total_lines; i++) {
        len += strlen(editor.lines[i]) + 1;
    }

    char *src = malloc(len + 1);
    if (!src)
        return;

    size_t pos = 0;
    for (int i = 0; i < editor.total_lines; i++) {
        size_t line_len = strlen(editor.lines[i]);
        memcpy(src + pos, editor.lines[i], line_len);
        pos += line_len;
        src[pos++] = '\n';
    }
    src[pos] = '\0';

    TSTree *new_tree = ts_parser_parse_string(parser, NULL, src, pos);
    if (!new_tree) {
        free(src);
        return;
    }

    if (tree)
        ts_tree_delete(tree);
    tree = new_tree;

    free(src);
}

static bool is_keyword(const char *text) {
    if (strcmp(file_ext, ".c") == 0) {
        for (int i = 0; c_keywords[i]; i++) {
            if (strcmp(text, c_keywords[i]) == 0) {
                return true;
            }
        }
    } else if (strcmp(file_ext, ".py") == 0) {
        for (int i = 0; py_keywords[i]; i++) {
            if (strcmp(text, py_keywords[i]) == 0) {
                return true;
            }
        }
    }

    return false;
}

static char *get_node_text(TSNode node) {
    TSPoint start = ts_node_start_point(node);
    TSPoint end = ts_node_end_point(node);
    if (start.row != end.row)
        return NULL;

    int len = end.column - start.column;
    char *buf = malloc(len + 1);
    memcpy(buf, editor.lines[start.row] + start.column, len);
    buf[len] = '\0';
    return buf;
}

static int color_for_node_type_c(const char *type) {
    if (strcmp(type, "primitive_type") == 0 ||
        strcmp(type, "type_identifier") == 0)
        return PAIR_TYPE;
    if (strcmp(type, "preproc_directive") == 0 ||
        strcmp(type, "preproc_call") == 0)
        return PAIR_PREPROCESSOR;
    if (strcmp(type, "system_lib_string") == 0)
        return PAIR_STRING;
    if (strcmp(type, "string_literal") == 0)
        return PAIR_STRING;
    if (strcmp(type, "number_literal") == 0 || strcmp(type, "number") == 0)
        return PAIR_NUM;
    if (strcmp(type, "char_literal") == 0)
        return PAIR_CHAR;
    if (strcmp(type, "comment") == 0)
        return PAIR_COMMENT;

    return PAIR_DEFAULT;
}

static int color_for_node_type_py(const char *type) {
    if (strcmp(type, "comment") == 0) {
        return PAIR_COMMENT;
    }
    if (strcmp(type, "function_definition") == 0) {
        return PAIR_FUNCTION;
    }
    if (strcmp(type, "class_definition") == 0) {
        return PAIR_TYPE;
    }
    if (strcmp(type, "string") == 0 || strcmp(type, "string_literal") == 0) {
        return PAIR_STRING;
    }
    if (strcmp(type, "number") == 0) {
        return PAIR_NUM;
    }

    return PAIR_DEFAULT;
}

static int color_for_node_type_lang(const char *type) {
    if (strcmp(file_ext, ".c") == 0) {
        return color_for_node_type_c(type);
    }
    if (strcmp(file_ext, ".py") == 0) {
        return color_for_node_type_py(type);
    }

    return PAIR_DEFAULT;
}

int get_color_for_pos(int line, int col) {
    if (!tree)
        return PAIR_DEFAULT;

    const char *text_line = editor.lines[line];

    TSNode root = ts_tree_root_node(tree);
    TSPoint point = { .row = (uint32_t)line, .column = (uint32_t)col };
    TSNode node = ts_node_descendant_for_point_range(root, point, point);

    if (ts_node_is_named(node)) {

        const char *node_type = ts_node_type(node);
        if (strcmp(node_type, "identifier") == 0) {
            TSNode parent = ts_node_parent(node);
            if (!ts_node_is_null(parent)) {
                const char *parent_type = ts_node_type(parent);

                if (strcmp(file_ext, ".c") == 0) {
                    if (strcmp(parent_type, "function_declarator") == 0 ||
                        strcmp(parent_type, "call_expression") == 0) {
                        return PAIR_FUNCTION;
                    }
                } else if (strcmp(file_ext, ".py") == 0) {
                    if (strcmp(parent_type, "call") == 0) {
                        return PAIR_FUNCTION;
                    }
                }

            }
        }

        int color = color_for_node_type_lang(node_type);
        if (color != PAIR_DEFAULT)
            return color;
    }

    TSNode parent = node;
    while (!ts_node_is_null(parent)) {
        if (ts_node_is_named(parent)) {
            int color = color_for_node_type_lang(ts_node_type(parent));
            if (color != PAIR_DEFAULT) {
                return color;
            }
        }
        parent = ts_node_parent(parent);
    }


    char *text = get_node_text(node);
    if (text && is_keyword(text)) {
        free(text);
        return PAIR_KEYWORD;
    }
    free(text);

    // Preprocessor
    int i = 0;
    while (text_line[i] == ' ' || text_line[i] == '\t')
        i++;
    if (text_line[i] == '#') {
        int start = i;
        while (text_line[i] && text_line[i] != ' ' && text_line[i] != '\t')
            i++;
        if (col >= start && col < i)
            return PAIR_PREPROCESSOR;
    }

    return PAIR_DEFAULT;
}
