#include "deletion.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cursor.h"
#include "editor.h"

void normalize_range(Point *a, Point *b) {
    if (a->y > b->y || (a->y == b->y && a->x > b->x)) {
        Point tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

bool is_at_line_start() { return cursor.x - editor.margin <= 0; }

void merge_lines(int dst, int src) {
    char *a = editor.lines[dst];
    char *b = editor.lines[src];

    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    a = realloc(a, len_a + len_b + 1);
    if (!a)
        return;

    strcat(a, b);
    free(editor.lines[src]);
    editor.lines[dst] = a;

    for (int i = src; i < editor.total_lines - 1; i++) {
        editor.lines[i] = editor.lines[i + 1];
    }

    editor.total_lines--;
}

bool delete_tab(Point pos) {
    char *line = editor.lines[pos.y];
    if (pos.x + 1 < editor.indent_size)
        return false;

    int count = 0;
    for (int i = pos.x; i >= 0; i--) {
        if (line[i] == ' ') {
            count++;
        } else {
            break;
        }
    }

    if (count >= editor.indent_size) {
        size_t tail_len = strlen(line + pos.x) + 1;
        int delete_size = editor.indent_size - (count % editor.indent_size);

        memmove(&line[pos.x - delete_size + 1], &line[pos.x + 1], tail_len);
        cursor.x -= delete_size;
        return true;
    }
    return false;
}

bool delete_pair(Point pos) {
    char *line = editor.lines[pos.y];
    char open = line[pos.x];
    char close = line[pos.x + 1];

    bool match =
        (open == '(' && close == ')') || (open == '[' && close == ']') ||
        (open == '{' && close == '}') || (open == '"' && close == '"') ||
        (open == '\'' && close == '\'');

    if (!match) {
        return false;
    }

    memmove(&line[pos.x], &line[pos.x + 2], strlen(line + pos.x + 2) + 1);
    cursor.x = pos.x + editor.margin;
    return true;
}

void delete_char(Point pos) {
    char *line = editor.lines[pos.y];
    int len = strlen(line);

    if (pos.x >= len) {
        return;
    }

    memmove(&line[pos.x], &line[pos.x + 1], len - pos.x);
    line[len - 1] = '\0';
    cursor.x = pos.x + editor.margin;
}

void delete_range(Point start, Point end) {
    if (start.y == end.y) {
        char *line = editor.lines[start.y];
        memmove(&line[start.x], &line[end.x], strlen(line + end.x) + 1);
        cursor.x = start.x + editor.margin;
        return;
    }

    char *first = editor.lines[start.y];
    char *last = editor.lines[end.y];

    first[start.x] = '\0';
    char *rest = last + end.x;

    size_t new_len = strlen(first) + strlen(rest) + 1;
    char *merged = malloc(new_len);
    snprintf(merged, new_len, "%s%s", first, rest);

    free(editor.lines[start.y]);
    editor.lines[start.y] = merged;

    for (int i = start.y + 1; i <= end.y; i++) {
        free(editor.lines[i]);
    }

    for (int i = end.y + 1; i < editor.total_lines; i++) {
        editor.lines[start.y + 1 + (i - end.y - 1)] = editor.lines[i];
    }

    editor.total_lines -= (end.y - start.y);

    cursor.x = start.x + editor.margin;
    cursor.y = start.y - cursor.y_offset;
}
