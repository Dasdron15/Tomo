#include "cursor.h"

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "editor.h"
#include "select.h"
#include "common.h"

CursorState cursor;

void move_up(bool is_selecting) {
    if (cursor.y + cursor.y_offset > 0) { 
        cursor.y--;
        cursor.x = cursor.max_x;
    } else {
        cursor.x = editor.margin;
        cursor.max_x = cursor.x;
    }
    
    if (!is_selecting) {
        cancel_selection();
    }

    clamp_cursor();
}

void move_down(bool is_selecting) {
    if (cursor.y + cursor.y_offset < editor.total_lines - 1) {
        cursor.y++;
        cursor.x = cursor.max_x;
    } else {
        size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
        cursor.x = line_len + editor.margin;
        cursor.max_x = cursor.x;
    }

    if (!is_selecting) {
        cancel_selection();
    }

    clamp_cursor();
}

void move_right(bool is_selecting) {
    int line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);

    if (line_len <= cursor.x - editor.margin && cursor.y + cursor.y_offset < editor.total_lines - 1) {
        cursor.x = editor.margin;
        cursor.y++;
    } else {
        cursor.x++;
    }

    cursor.max_x = cursor.x;

    if (!is_selecting) {
        cancel_selection();
    }

    clamp_cursor();
}

void move_left(bool is_selecting) {
    if (cursor.x > editor.margin) {
        cursor.x--;
    } else if (cursor.y > 0) {
        cursor.y--;
        size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
        editor.margin = int_len(editor.total_lines) + 2;
        cursor.x = line_len + editor.margin;
    }

    cursor.max_x = cursor.x;

    if (!is_selecting) {
        cancel_selection();
    }

    clamp_cursor();
}

void clamp_cursor(void) {
    int line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
    int screen_width = getmaxx(stdscr);
    int screen_height = getmaxy(stdscr);

    if (cursor.x < editor.margin) {
        cursor.x = editor.margin;
    } else if (cursor.x + cursor.x_offset > line_len + editor.margin) {
        cursor.x = line_len + editor.margin;
    }
    
    if (cursor.y < 3 && cursor.y_offset > 0) {
        if (cursor.y_offset - (3 - cursor.y) < 0) {
            cursor.y_offset = 0;
            cursor.y = is_selecting() ? get_start().y : 0;
        } else {
            cursor.y_offset -= 3 - cursor.y;
            cursor.y = 3;
        }
    }

    if (cursor.y > screen_height - 5) {
        cursor.y_offset += cursor.y - (screen_height - 5);
        cursor.y = screen_height - 5;
    }
}

size_t goto_line() {
    const size_t height = 3;
    const size_t width = 30;

    WINDOW *box = newwin(height, width, 1, getmaxx(stdscr) - 33);

    box(box, 0, 0);
    mvwprintw(box, 1, 2, "Go to line: ");

    curs_set(1);

    char input[15] = {'\0'};
    int pos = 0;

    int ch;
    while ((ch = wgetch(box)) != '\n') {
        if (ch == 17) {
            if (!editor.is_saved) {
                ask_for_save();
            }
            endwin();
            exit(0);
            return 0;
        }

        if (ch == 27) {
            delwin(box);
            return -1;
        } else if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') &&
                   pos > 0) {
            input[--pos] = '\0';
            mvwprintw(box, 1, 14 + pos, " ");
            wmove(box, 1, 14 + pos);
        } else if (isdigit(ch) && pos < (int)sizeof(input) - 1) {
            input[pos++] = ch;
            mvwprintw(box, 1, 13 + pos, "%c", ch);
            wmove(box, 1, 14 + pos);
        }
        wrefresh(box);
    }

    delwin(box);

    int line = atoi(input);
    if (line < 1) {
        return -1;
    }

    if (line > editor.total_lines) {
        return editor.total_lines - 1;
    }
    return line - 1;
}

