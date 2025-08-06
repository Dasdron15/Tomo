#include "editor.h"

#include <stdlib.h>
#include <string.h>

#include <curses.h>

#include "draw.h"
#include "fileio.h"

EditorState editor;

void draw_editor() {
    erase();

    draw_text_area();
    draw_status_bar();
    draw_bottom_text();
    place_cursor();
}

void ask_for_save(void) {
    move(getmaxy(stdscr) - 1, 0);
    clrtoeol();

    attron(A_BOLD);
    mvwprintw(stdscr, getmaxy(stdscr) - 1, 0, "Save changes? (y/n): ");
    attroff(A_BOLD);

    curs_set(1);

    char input[16] = {0};
    int pos = 0;

    int ch;
    while ((ch = wgetch(stdscr)) != '\n') {
        if (ch == 27) {
            editor.bottom_text = "";
            return;
        } else if ((ch == KEY_BACKSPACE || ch == 127) && pos > 0) {
            input[--pos] = '\0';
            mvwprintw(stdscr, getmaxy(stdscr) - 1, 21 + pos, " ");
            wmove(stdscr, getmaxy(stdscr) - 1, 21 + pos);
        } else if (pos < (int)sizeof(input) - 1 && ch < 127 && ch > 31) {
            input[pos++] = ch;
            mvwprintw(stdscr, getmaxy(stdscr) - 1, 20 + pos, "%c", ch);
            wmove(stdscr, getmaxy(stdscr) - 1, 21 + pos);
        }
        wrefresh(stdscr);
    }

    if (strcasecmp(input, "y") == 0 || strcasecmp(input, "yes") == 0) {
        reset();
        save_file();
        endwin();
        exit(0);
        return;
    }

    if (strcasecmp(input, "n") == 0 || strcasecmp(input, "no")) {
        reset();
        endwin();
        exit(0);
        return;
    }
}

void is_saved() {
    FILE
}

void reset(void) {
    printf("\033[2 q");
    fflush(stdout);
}
