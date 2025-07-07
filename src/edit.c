#include "edit.h"

#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "cursor.h"
#include "editor.h"
#include "select.h"

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
    if ((start.y == end.y && is_selecting()) ||
        (!is_selecting())) {
        if (start.x < 0 && start.y > 0) {
            editor.total_lines--;            
            move_left(false);

            char *new_line = realloc(editor.lines[start.y - 1],
                                     strlen(editor.lines[start.y - 1]) +
                                         strlen(editor.lines[start.y]) + 1);
            editor.lines[start.y - 1] = new_line;

            strcat(editor.lines[start.y - 1], editor.lines[start.y]);

            for (int i = start.y; i < editor.total_lines; i++) {
                editor.lines[i] = editor.lines[i + 1];
            }

            return;
        }

        int remove_count = end.x - start.x + 1;
        int length = strlen(editor.lines[start.y]);

        memmove(&editor.lines[start.y][start.x],
                &editor.lines[end.y][end.x + 1], length - end.x);
        editor.lines[start.y][length - remove_count] = '\0';

        cursor.x = start.x + editor.margin;
    } else if (!(start.x < 0 && start.y == 0)) {
        int deletion_range = end.y - start.y;

        if (start.x > 0) {
            editor.lines[start.y][start.x] = '\0';
        }

        char *line = editor.lines[end.y];
        memmove(line, line + end.x + 1, strlen(line) - end.x);

        char *new_line = realloc(editor.lines[start.y],
                                 strlen(editor.lines[start.y]) + strlen(line));
        editor.lines[start.y] = new_line;
        strcat(editor.lines[start.y], editor.lines[end.y]);

        for (int i = end.y + 1; i < editor.total_lines; i++) {
            editor.lines[start.y + 1 + (i - end.y - 1)] = editor.lines[i];
        }

        editor.total_lines -= deletion_range;

        cursor.x = start.x + editor.margin;
        cursor.y = start.y - cursor.y_offset;
    }

    editor.margin = int_len(editor.total_lines) + 2;

    if (cursor.x < editor.margin) {
        cursor.x = editor.margin;
    }

    if (cursor.y < 0) {
        cursor.y = 0;
    }

    cancel_selection();
    clamp_cursor();

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
