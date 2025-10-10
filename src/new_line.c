#include "new_line.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "editor.h"
#include "cursor.h"

/*
 * Calculates indentation level of the given line.
 * If the file uses tabs, counts the number of leading tab characters.
 * If the file uses spaces, counts leading spaces and multiplies by editor.indent_size.
 * If the previous character on the line is ':', increases indentation by one level.
 */

int calculate_indent(const char *line) {
    int indent = 0;
    int len = strlen(line);
    int index = cursor.y + cursor.y_offset;
    int pos = cursor.x + cursor.x_offset - editor.margin;

    char indent_symbol = ' ';

    if (editor.tab_indent) {
        indent_symbol = '\t';
    }
    
    for (int i = 0; i < len && line[i] == indent_symbol; i++) {
        indent++;
    }

    if (pos && editor.lines[index][pos - 1] == ':') {
        if (indent_symbol == '\t') {
            indent++;
        } else {
            indent += editor.indent_size;
        }
    } 
    
    return indent;
}

/*
 * Takes characters to the left and right of the cursor as arguments.
 * Returns true if the cursor is inside a matching pair of parentheses, brackets, or braces.
 */

bool is_enclosing_pair(char left, char right) {
    return (left == '(' && right == ')') ||
           (left == '[' && right == ']') ||
           (left == '{' && right == '}');
}

/*
 * Creates a new line string based on the current indentation level.
 * Takes the remaining part of the current line and appends it to the proper indentation.
 * Used when inserting a new line after pressing Enter.
 */

char *create_right_side(const char *rest_of_line, int indent) {
    char indent_symb;

    if (editor.tab_indent) {
        indent_symb = '\t';
    } else {
        indent_symb = ' ';
    }
    char *indent_str = mult_char(indent_symb, indent);
    size_t total_len = strlen(indent_str) + strlen(rest_of_line) + 1;
    char *result = malloc(total_len);
    if (!result)
        return NULL;

    strcpy(result, indent_str);
    strcat(result, rest_of_line);
    free(indent_str);
    return result;
}
