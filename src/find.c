#include "find.h"

#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "draw.h"
#include "editor.h"
#include "input.h"
#include "cursor.h"
#include "select.h"

void search(char *query) {
    for (int i = 0; i < editor.total_lines; i++) {
        char *pos = strstr(editor.lines[i], query);
        if (pos) {
            cursor.y = i - cursor.y_offset;
            cursor.x = pos - editor.lines[i] + editor.margin - cursor.x_offset;

            Point start_sel_point = {cursor.y + cursor.y_offset, 
                                     cursor.x + cursor.x_offset - editor.margin};
            Point end_sel_point = {cursor.y + cursor.y_offset,
                                   cursor.x + cursor.x_offset + strlen(query) - editor.margin + 1};

            place_cursor();
            clamp_cursor();

            set_selection(start_sel_point, end_sel_point);
            break;
        }

        cancel_selection();
    }
}

void find(void) {
    int screen_height = getmaxy(stdscr);
    char *prompt = "Find: ";

    move(getmaxy(stdscr) - 1, 0);
    clrtoeol();

    mvprintw(screen_height - 1, 0, "%s", prompt);

    curs_set(1);

    char input[32] = {0};
    int pos = 0;

    int ch;
    while ((ch = wgetch(stdscr)) != '\n') {
        draw_editor();
        mvprintw(screen_height - 1, 0, "%s", prompt);

        if (ch == CTRL('q')) {
            exit_editor();
        }

        if (ch == KEY_ESCAPE) {
            return;
        }

        if ((ch == KEY_BACKSPACE || ch == KEY_DELETE || ch == '\b') && pos > 0) {
            input[--pos] = '\0';
            mvprintw(screen_height - 1, strlen(prompt), "%s", input);
            move(screen_height - 1, strlen(prompt) + pos);
        }

        if (ch >= ' ' && ch < KEY_DELETE && pos < (int)sizeof(input) - 1) {
            input[pos++] = ch;
            mvprintw(screen_height - 1, strlen(prompt), "%s", input);
            move(screen_height - 1, strlen(prompt) + pos);
        }

        search(input);
    }
}

void find_replace(void) {

}
