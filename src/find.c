#include "find.h"

#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "editor.h"
#include "input.h"

void find(void) {
    int screen_height = getmaxy(stdscr);
    char *prompt = "Search: ";

    move(getmaxy(stdscr) - 1, 0);
    clrtoeol();

    mvprintw(screen_height - 1, 0, "%s", prompt);

    curs_set(1);

    char input[16] = {0};
    int pos = 0;

    int ch;
    while ((ch = wgetch(stdscr)) != '\n') {
        if (ch == CTRL('q')) {
            exit_editor();
        }

        if (ch == KEY_ESCAPE) {
            return;
        }

        if ((ch == KEY_BACKSPACE || ch == KEY_DELETE || ch == '\b') && pos > 0) {
            input[--pos] = '\0';
            mvprintw(screen_height - 1, strlen(prompt) + pos, " ");
            move(screen_height - 1, strlen(prompt) + pos);
        }

        if (isdigit(ch) && pos < (int)sizeof(input) - 1) {
            input[pos++] = ch;
            mvprintw(screen_height - 1, strlen(prompt) + pos - 1, "%c", ch);
            move(screen_height - 1, strlen(prompt) + pos);
        }
    }
}

void find_replace(void) {

}
