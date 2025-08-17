#include "editor.h"

#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "draw.h"
#include "fileio.h"
#include "utils.h"

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

    mvwprintw(stdscr, getmaxy(stdscr) - 1, 0, "Save changes? (y/n): ");

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
        if (!save_file()) {
            return;
        }

        reset();
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

bool is_saved(void) {
    int unsaved_len = 1;
    for (int i = 0; i < editor.total_lines; i++) {
        unsaved_len += strlen(editor.lines[i]) + 1;
    }

    char *unsaved = malloc(unsaved_len);
    if (!unsaved) return false;
    unsaved[0] = '\0';

    for (int i = 0; i < editor.total_lines; i++) {
        strcat(unsaved, editor.lines[i]);

        // Adds /n to every line except the last one
        if (i + 1 < editor.total_lines) {
            strcat(unsaved, "\n");
        }
    }

    char *saved = file_to_string(editor.filename);
    if (!saved) {
        free(unsaved);
        return false;
    }

    bool is_equal = (strcmp(unsaved, saved) == 0);

    free(unsaved);
    free(saved);

    return is_equal;
}

void exit_editor(void) {
    if (!is_saved()) {
        ask_for_save();
        return;
    }
    endwin();
    reset();
    exit(0);
}

void reset(void) {
    printf("\033[2 q");
    fflush(stdout);
}
