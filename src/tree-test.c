#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <tree_sitter/api.h>

TSLanguage *tree_sitter_c();

char *read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);
    return buffer;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Error: not enough arguments");
        return 1;
    }
    
    // Create a parser.
    TSParser *parser = ts_parser_new();

    // Set the parser's language (C in this case).
    ts_parser_set_language(parser, tree_sitter_c());

    // Build a syntax tree based on source code stored in a string.
    const char *source_code = read_file(argv[1]);

    printf("Code parsed: %s\n", source_code);

    TSTree *tree =
        ts_parser_parse_string(parser, NULL, source_code, strlen(source_code));

    TSNode root_node = ts_tree_root_node(tree);

    TSNode array_node = ts_node_named_child(root_node, 0);
    char *string = ts_node_string(root_node);
    printf("Syntax tree: %s\n", string);

    // Free all of the heap-allocated memory.
    free(string);
    ts_tree_delete(tree);
    ts_parser_delete(parser);
    return 0;
}
