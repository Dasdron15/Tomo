#include "edit.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "cursor.h"
#include "editor.h"
#include "select.h"
#include "clip.h"

void add_tab(void) {
    int pos = cursor.x - editor.margin;

    char *old = editor.lines[cursor.y + cursor.y_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
        editor.lines[cursor.y + cursor.y_offset] = old;
    }

    char *tab_str = mult_char(' ', TAB_SIZE);

    if (!tab_str) {
        return;
    }

    char *new = malloc(strlen(old) + TAB_SIZE + 1);

    if (new == NULL) {
        free(tab_str);
        return;
    }

    strncpy(new, old, pos);
    memcpy(new + pos, tab_str, TAB_SIZE);
    strcpy(new + pos + TAB_SIZE, old + pos);

    editor.lines[cursor.y + cursor.y_offset] = new;
    free(old);
    free(tab_str);

    cursor.x += TAB_SIZE;

    cursor.max_x = cursor.x;

    clamp_cursor();
}


void insert_char(char c) {
    int pos = cursor.x + cursor.x_offset - editor.margin;

    char *old = editor.lines[cursor.y + cursor.y_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
        editor.lines[cursor.y + cursor.y_offset] = old;
    }

    char *new = malloc(strlen(old) + 2);
    memset(new, 0, strlen(old) + 2);

    if (new == NULL) {
        return;
    }

    strncpy(new, old, pos);
    new[pos] = c;
    strcpy(new + pos + 1, old + pos);

    editor.lines[cursor.y + cursor.y_offset] = new;

    free(old);
    move_right(false);

    cursor.max_x = cursor.x;
}

void deletion(Point start, Point end) {
    if (start.y >= editor.total_lines || end.y >= editor.total_lines) return;
    if (start.x < 0) start.x = 0;
    if (end.x < 0) end.x = 0;

    // Start less than or equal to end
    if (start.y > end.y || (start.y == end.y && start.x > end.x)) {
        Point tmp = start;
        start = end;
        end = tmp;
    }

    // 1. One line deletion
    if (start.y == end.y) {
        char* line = editor.lines[start.y];
        int len = strlen(line);

        if (start.x >= len) return;

        memmove(&line[start.x], &line[end.x + 1], len - end.x);
        line[len - (end.x - start.x + 1)] = '\0';

        cursor.x = start.x + editor.margin;
    }
    // 2. Multiple line deletion
    else {
        char* first = editor.lines[start.y];
        char* last = editor.lines[end.y];

        first[start.x] = '\0';
        char* rest = &last[end.x + 1];

        size_t new_len = strlen(first) + strlen(rest) + 1;
        char* merged = malloc(new_len);
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
    
    cancel_selection();
    clamp_cursor();

    editor.margin = int_len(editor.total_lines) + 2;

    if (cursor.x < editor.margin) cursor.x = editor.margin;
    if (cursor.y < 0) cursor.y = 0;

    cursor.max_x = cursor.x;
}

void new_line(void) {
    int pos = cursor.x + cursor.x_offset - editor.margin;
    int y_pos = cursor.y + cursor.y_offset;

    char *current = editor.lines[y_pos];
    int current_len = strlen(current);

    char *left = malloc(pos + 1);
    char *right = malloc(current_len - pos + 1);

    if (pos > current_len) {
        pos = current_len;
    }

    if (!left || !right) {
        return;
    }

    strncpy(left, current, pos);
    left[pos] = '\0';

    strcpy(right, current + pos);

    free(editor.lines[y_pos]);
    editor.lines[y_pos] = NULL;

    for (int i = editor.total_lines; i > y_pos; i--) {
        editor.lines[i] = editor.lines[i - 1];
    }

    editor.lines[y_pos] = left;
    editor.lines[y_pos + 1] = right;

    editor.total_lines++;

    cursor.y++;
    editor.margin = int_len(editor.total_lines) + 2;
    cursor.x = editor.margin;
    cursor.x_offset = 0;
    cursor.max_x = cursor.x;

    clamp_cursor();
}

void copy_text(Point start, Point end) {
    char* clipboard = malloc(1);
    clipboard[0] = '\0';
    size_t clip_len = 0;

    for (int y = start.y; y <= end.y; y++) {
        int from = (y == start.y) ? start.x : 0;
        int to = (y == end.y) ? end.y : strlen(editor.lines[y]) - 1;
        
        for (int x = from; x <= to; x++) {
            char ch = editor.lines[y][x];
            clipboard = realloc(clipboard, clip_len + 2);
            clipboard[clip_len++] = ch;
            clipboard[clip_len] = '\0';
        }

        if (y < end.y) {
            clipboard = realloc(clipboard, clip_len + 2);
            clipboard[clip_len++] = '\n';
            clipboard[clip_len] = '\0';
        }
    }

    set_clipboard(clipboard);
}

