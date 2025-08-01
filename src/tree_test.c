#include "tree_sitter/api.h"
#include <stdio.h>
#include <stdlib.h>

const TSLanguage *tree_sitte_c(void);

char *read_file(const char *path) {
    FILE *fp = fopen(path, "r");
    char *text = malloc(1);

    char buf[64];
    while (())
}

int main(void) {
    TSParser *parser = ts_parser_new();

    ts_parser_set_language(parser, tree_sitte_c());
}
