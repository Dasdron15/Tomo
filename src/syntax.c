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
    for (int i = 0; i < editor.total_lines; i++) {
        strcat(src, editor.lines[i]);
        strcat(src, "\n");
    }

    TSTree *new_tree = ts_parser_parse_string(parser, tree, src, strlen(src));
    if (tree) ts_tree_delete(tree);
    tree = new_tree;

    free(src);
}

static int color_for_node_type(const char *type) {
    if (strcmp(type, "primitive_type") == 0 || strcmp(type, "type_identifier")) return PAIR_TYPE;
    if (strcmp(type, "string_literal") == 0) return PAIR_STRING;
    if (strcmp(type, "number_literal") == 0 || strcmp(type, "number") == 0) return PAIR_NUM;
    if (strcmp(type, "char_literal") == 0) return PAIR_CHAR;
    if (strcmp(type, "function_definition") == 0) return PAIR_FUNCTION;
    if (strcmp(type, "preproc_directive") == 0 || strcmp(type, "preproc_include")) return PAIR_UNACTIVE;
    if (strcmp(type, "comment") == 0) return PAIR_UNACTIVE;

    return PAIR_DEFAULT;
}

int get_color_for_pos(int line, int col) {
    if (!tree) return PAIR_DEFAULT;

    TSNode root = ts_tree_root_node(tree);

    TSPoint point = { .row = (uint32_t)line, .column = (uint32_t)col };
    TSNode node = ts_node_descendant_for_point_range(root, point, point);
    if (!ts_node_is_named(node)) return PAIR_DEFAULT;

    const char *type = ts_node_type(node);

    return color_for_node_type(type);
}
