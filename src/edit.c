#include "edit.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "clip.h"
#include "utils.h"
#include "cursor.h"
#include "editor.h"
#include "select.h"
#include "new_line.h"
#include "deletion.h"

void add_tab(void) {
    int pos = cursor.x + cursor.x_offset - editor.margin;

    char *old = editor.lines[cursor.y + cursor.y_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
        editor.lines[cursor.y + cursor.y_offset] = old;
    }

    char *tab_str;
    int tab_len;

    if (editor.tab_indent) {
        tab_str = "\t";
        tab_len = 1;
    } else {
        tab_str = mult_char(' ', editor.indent_size);
        tab_len = editor.indent_size;
    }

    char *new = malloc(strlen(old) + tab_len + 1);
    
    if (new == NULL) {
        free(tab_str);
        return;
    }

    strncpy(new, old, pos);
    memcpy(new + pos, tab_str, tab_len);
    strcpy(new + pos + tab_len, old + pos);

    editor.lines[cursor.y + cursor.y_offset] = new;

    if (!editor.tab_indent) {
        free(tab_str);
    }

    cursor.x += tab_len;
    cursor.max_x = cursor.x + cursor.x_offset;

    clamp_cursor();
}

void insert_char(char c) {
    int pos = cursor.x + cursor.x_offset - editor.margin;
    char *new;

    char *old = editor.lines[cursor.y + cursor.y_offset];

    if (old == NULL) {
        old = malloc(1);
        old[0] = '\0';
        editor.lines[cursor.y + cursor.y_offset] = old;
    }

    if (c == '\'' || c == '"' || c == '{' || c == '[' || c == '(') {
        char close_ch;
        switch (c) {
        case '\'':
            close_ch = '\'';
            break;
        case '"':
            close_ch = '"';
            break;
        case '{':
            close_ch = '}';
            break;
        case '[':
            close_ch = ']';
            break;
        case '(':
            close_ch = ')';
            break;
        }

        new = malloc(strlen(old) + 3);
        memset(new, 0, strlen(old) + 3);

        if (new == NULL) {
            return;
        }

        strncpy(new, old, pos);
        new[pos] = c;
        new[pos + 1] = close_ch;
        strcpy(new + pos + 2, old + pos);
    } else {
        new = malloc(strlen(old) + 2);
        memset(new, 0, strlen(old) + 2);

        if (new == NULL) {
            return;
        }

        strncpy(new, old, pos);
        new[pos] = c;
        strcpy(new + pos + 1, old + pos);
    }

    editor.lines[cursor.y + cursor.y_offset] = new;

    free(old);
    move_right(false);

    cursor.max_x = cursor.x + cursor.x_offset;
}

void deletion(Point start, Point end) {
    if (start.y >= editor.total_lines || end.y >= editor.total_lines) return;

    normalize_range(&start, &end);

    if (is_selecting()) {
        delete_range(start, end);
    } else if (is_at_line_start() && cursor.y > 0) {
        size_t prev_len = strlen(editor.lines[start.y - 1]);
        merge_lines(cursor.y + cursor.y_offset - 1, cursor.y + cursor.y_offset);
        cursor.y--;
        cursor.x = prev_len + editor.margin;
    } else if (delete_tab(start)) {
        // Poop
    } else if (delete_pair(start)) {
        // Poop
    } else if (!is_at_line_start()) {
        delete_char(start);
    }

    cancel_selection();
    clamp_cursor();

    editor.margin = int_len(editor.total_lines) + 2;
    cursor.max_x = cursor.x + cursor.x_offset;
}

void new_line(void) {
    char indent_symb;
    
    if (editor.tab_indent) {
        indent_symb = '\t';
    } else {
        indent_symb = ' ';
    }
    
    int x_pos = cursor.x + cursor.x_offset - editor.margin;
    int y_pos = cursor.y + cursor.y_offset;

    char *line = editor.lines[y_pos];
    int indent = calculate_indent(line);

    char left = (x_pos > 0) ? line[x_pos - 1] : '\0';
    char right = line[x_pos];
    
    if (is_enclosing_pair(left, right)) {
        indent += editor.indent_size;

        char *line_before = strndup(line, x_pos);
        char *middle = mult_char(indent_symb, indent);
        char *line_after = create_right_side(line + x_pos, indent - editor.indent_size);

        memmove(&editor.lines[y_pos + 3], &editor.lines[y_pos + 1], (editor.total_lines - y_pos - 1) * sizeof(char*));

        free(editor.lines[y_pos]);
        
        editor.lines[y_pos] = line_before;
        editor.lines[y_pos + 1] = middle;
        editor.lines[y_pos + 2] = line_after;

        editor.total_lines += 2;
    } else {
        char *line_before = strndup(line, x_pos);
        char *right = create_right_side(line + x_pos, indent);

        memmove(&editor.lines[y_pos + 1], &editor.lines[y_pos], (editor.total_lines - y_pos) * sizeof(char*));
        editor.lines[y_pos] = line_before;
        editor.lines[y_pos + 1] = right;

        editor.total_lines++;
    }

    cursor.y++;
    editor.margin = int_len(editor.total_lines) + 2;
    cursor.x = editor.margin + indent - cursor.x_offset;
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

void paste_text(void) {
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
