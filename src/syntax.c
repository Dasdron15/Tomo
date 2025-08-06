#include "syntax.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <curses.h>

#include "editor.h"
#include "init.h"
#include "tree_sitter/api.h"

const TSLanguage *tree_sitter_c();

static TSParser *parser = NULL;
static TSTree *tree = NULL;

void syntax_init(void) {
    parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_c());
}

void syntax_reparse(void) {
    size_t len = 0;
    for (int i = 0; i < editor.total_lines; i++) {
        len += strlen(editor.lines[i]) + 1;
    }

    char *src = malloc(len + 1);
    if (!src) return;

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

    if (tree) ts_tree_delete(tree);
    tree = new_tree;

    free(src);
}

static int color_for_node_type(const char *type) {
    if (strcmp(type, "primitive_type") == 0 || strcmp(type, "type_identifier") == 0) return PAIR_TYPE;
    if (strcmp(type, "preproc_directive") == 0 || strcmp(type, "preproc_call") == 0) return PAIR_PREPROCESSOR;
    if (strcmp(type, "system_lib_string") == 0) return PAIR_STRING;
    if (strcmp(type, "string_literal") == 0) return PAIR_STRING;
    if (strcmp(type, "number_literal") == 0 || strcmp(type, "number") == 0) return PAIR_NUM;
    if (strcmp(type, "char_literal") == 0) return PAIR_CHAR;
    if (strcmp(type, "comment") == 0) return PAIR_COMMENT;
    return PAIR_DEFAULT;
}

int get_color_for_pos(int line, int col) {
    if (!tree) return PAIR_DEFAULT;

    const char *text_line = editor.lines[line];

    TSNode root = ts_tree_root_node(tree);
    TSPoint point = { .row = (uint32_t)line, .column = (uint32_t)col };
    TSNode node = ts_node_descendant_for_point_range(root, point, point);

    if (ts_node_is_named(node) && strcmp(ts_node_type(node), "identifier") == 0) {
        TSNode parent = ts_node_parent(node);
        if (!ts_node_is_null(parent)) {
            const char *parent_type = ts_node_type(parent);

            if (strcmp(parent_type, "function_declarator") == 0) {
                return PAIR_FUNCTION;
            }
            if (strcmp(parent_type, "call_expression") == 0) {
                return PAIR_FUNCTION;
            }
        }

    }

    while (!ts_node_is_null(node)) {
        if (ts_node_is_named(node)) {
            int color = color_for_node_type(ts_node_type(node));
            if (color != PAIR_DEFAULT) {
                return color;
            }
        }
        node = ts_node_parent(node);
    }

    // Preprocessors!!!
    const char *line_text = editor.lines[line];
    int i = 0;
    while (line_text[i] == ' ' || line_text[i] == '\t') i++;
    if (text_line[i] == '#') {
        int start = i;
        while (text_line[i] && text_line[i] != ' ' && text_line[i] != '\t') i++;
        if (col >= start && col < i) {
            return PAIR_PREPROCESSOR;
        }
    }

    return PAIR_DEFAULT;
}
