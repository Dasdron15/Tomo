#include "edit.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clip.h"
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

    cursor.max_x = cursor.x + cursor.x_offset;

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

    cursor.max_x = cursor.x + cursor.x_offset;
}

void deletion(Point start, Point end) {
    if (start.y >= editor.total_lines || end.y >= editor.total_lines ||
        (!is_selecting() && start.y <= 0 && start.x < 0))
        return;
    if (start.x < 0)
        start.x = 0;
    if (end.x < 0)
        end.x = 0;

    if (start.y > end.y || (start.y == end.y && start.x > end.x)) {
        Point tmp = start;
        start = end;
        end = tmp;
    }

    if (!is_selecting() && cursor.x - editor.margin - 1 < 0 && cursor.y > 0) {
        int prev_len = strlen(editor.lines[start.y - 1]);
        int curr_len = strlen(editor.lines[start.y]);

        editor.lines[start.y - 1] =
            realloc(editor.lines[start.y - 1], prev_len + curr_len + 1);
        strcat(editor.lines[start.y - 1], editor.lines[start.y]);
        free(editor.lines[start.y]);

        for (int i = start.y; i < editor.total_lines - 1; i++) {
            editor.lines[i] = editor.lines[i + 1];
        }

        editor.total_lines--;

        cursor.y--;
        cursor.x = prev_len + editor.margin;

    } else if (start.y == end.y) {
        char *line = editor.lines[start.y];
        int len = strlen(line);

        if (start.x >= len)
            return;

        if (end.x - start.x == len) { // If the whole line is selected
            editor.lines[start.y][start.x] = '\0';
        } else {
            memmove(&line[start.x], &line[end.x + 1], len - end.x);
            line[len - (end.x - start.x + 1)] = '\0';

            cursor.x = start.x + editor.margin;
        }
    } else {
        char *first = editor.lines[start.y];
        char *last = editor.lines[end.y];

        first[start.x] = '\0';
        char *rest = &last[end.x + 1];

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

    clamp_cursor();
    cancel_selection();

    editor.margin = int_len(editor.total_lines) + 2;

    cursor.max_x = cursor.x + cursor.x_offset;
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
    cursor.max_x = cursor.x + cursor.x_offset;

    clamp_cursor();
}

void copy_text(Point start, Point end) {
    char *clipboard = malloc(1);

    if (!clipboard) {
        return;
    }

    clipboard[0] = '\0';
    size_t clip_len = 0;

    for (int y = start.y; y <= end.y; y++) {
        int from = (y == start.y) ? start.x : 0;
        int to = (y == end.y) ? end.x : (int)strlen(editor.lines[y]);

        for (int x = from; x < to; x++) {
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

void paste_text() {
    char *clipboard = strdup(get_clipboard());
    if (!clipboard)
        return;

    int clipboard_lines = count_char(clipboard, '\n');
    editor.total_lines += clipboard_lines;

    int y_pos = cursor.y + cursor.y_offset;
    int x_pos = cursor.x + cursor.x_offset - editor.margin;
    char *current_line = editor.lines[y_pos];

    char *line_remainder = strdup(current_line + x_pos);
    current_line[x_pos] = '\0';

    if (clipboard_lines > 0) {
        int index = cursor.y + cursor.y_offset;

        // Tokenise clipboard to an array
        char **lines = split(clipboard, '\n');

        // Merge the current line and first clipboard line
        size_t new_len = strlen(current_line) + strlen(lines[0]) + 1;
        current_line = realloc(current_line, new_len);
        strcat(current_line, lines[0]);
        editor.lines[index] = current_line;

        // Shift lines in the main lines array
        for (int i = editor.total_lines - 1; i >= index; i--) {
            editor.lines[i + clipboard_lines] = editor.lines[i];
        }

        // Add the rest of the lines
        int i;
        for (i = 1; i < clipboard_lines + 1; i++) {
            editor.lines[i + index] = lines[i];
        }

        i--;
        size_t old_last_len = strlen(lines[i]);

        size_t new_last_len = strlen(lines[i]) + strlen(line_remainder) + 1;
        char *last_line = lines[i];
        last_line = realloc(last_line, new_last_len);
        strcat(last_line, line_remainder);
        editor.lines[i + index] = last_line;

        cursor.y += clipboard_lines;
        cursor.x = old_last_len + editor.margin;

        free(lines);
    } else {
        size_t new_len = strlen(current_line) + strlen(line_remainder) +
                         strlen(clipboard) + 1;
        current_line = realloc(current_line, new_len);

        strcat(current_line, clipboard);
        strcat(current_line, line_remainder);

        editor.lines[y_pos] = current_line;
        cursor.x += strlen(clipboard);
    }

    cursor.max_x = cursor.x + cursor.x_offset;

    free(line_remainder);
    free(clipboard);
    clamp_cursor();
}
