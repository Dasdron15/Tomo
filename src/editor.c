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

bool is_saved(void) {
    // Extract code from the opened file
    
    FILE *fp = fopen(editor.filename, "r");
    if (!fp) {
        endwin();
        fprintf(stderr, "Error: Cannot open file\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *saved_code = malloc(length + 1);
    if (!saved_code) {
        fclose(fp);
        endwin();
        fprintf(stderr, "Error: Memory not allocated\n");
        exit(0);
    }

    fread(saved_code, 1, length, fp);
    saved_code[length] = '\0';

    // Unsaved code
    size_t total_len = 0;
    for (size_t i = 0; i < editor.total_lines; i++) {
        total_len += strlen(editor.lines[i]) + 1;
    }

    char *unsaved_code = malloc(total_len + 1);
    if (!unsaved_code) {
        endwin();
        fprintf(stderr, "Error: Memory not allocated\n");
        exit(0);
    }

    unsaved_code[0] = '\0';
    for (size_t i = 0; i < editor.total_lines; i++) {
        strcat(unsaved_code, editor.lines[i]);
        strcat(unsaved_code, "\n");
    }

    if (strcmp(saved_code, unsaved_code) == 0) {
        return true;
    }
    
    return false;
}

void reset(void) {
    printf("\033[2 q");
    fflush(stdout);
}
