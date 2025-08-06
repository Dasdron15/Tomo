#include "draw.h"

#include <ctype.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "init.h"
#include "utils.h"
#include "cursor.h"
#include "editor.h"
#include "select.h"
#include "syntax.h"

int calc_render_x(const char *line, int cursor_x) {
    int render_x = 0;
    for (int i = 0; i < cursor_x; i++) {
        if (line[i] == '\t') {
            render_x += editor.tab_width;
        } else {
            render_x++;
        }
    }
    return render_x;
}

void draw_text_area(void) {
    int screen_width = getmaxx(stdscr);
    int screen_height = getmaxy(stdscr);

    int line_num_pos = 0;
    editor.margin = int_len(editor.total_lines) + 2;

    for (int index = cursor.y_offset;
         index < cursor.y_offset + screen_height - 2 &&
         index < editor.total_lines;
         index++) {

        char *line = editor.lines[index];
        draw_line_with_number(index, line_num_pos);
        draw_line_content(index, line, line_num_pos);

        line_num_pos++;
    }
}

void draw_line_with_number(int index, int y_pos) {
    char *spaces =
        mult_char(' ', int_len(editor.total_lines) - int_len(index + 1));

    if (index == cursor.y + cursor.y_offset) {
        mvprintw(y_pos, 0, " %s%d ", spaces, index + 1);
    } else {
        attron(COLOR_PAIR(PAIR_UNACTIVE));
        mvprintw(y_pos, 0, " %s%d ", spaces, index + 1);
        attroff(COLOR_PAIR(PAIR_UNACTIVE));
    }

    free(spaces);
}

void draw_line_content(int index, char *line, int y) {
    int screen_width = getmaxx(stdscr);
    int col = editor.margin;

    for (int symb = cursor.x_offset;
         symb <= (int) strlen(line) && symb < cursor.x_offset + screen_width;
         symb++) {
        int file_x = symb;
        int file_y = index;

        char ch = (symb < (int) strlen(line)) ? line[symb] : ' ';
        int tab_size =
            editor.tab_indent ? editor.tab_width : editor.indent_size;
        char *tab = mult_char(' ', tab_size);

        int color = get_color_for_pos(index, symb);

        int select_offset = 13;
        int color_pair = is_selected(file_y, file_x) ? color + select_offset : color;

        attron(COLOR_PAIR(color_pair));

        if (ch == '\t') {
            mvprintw(y, col, "%s", tab);
            col += tab_size - 1;
        } else {
            mvprintw(y, col, "%c", ch);
        }

        attroff(COLOR_PAIR(color_pair));
        col++;
        free(tab);
    }
}

void draw_status_bar(void) {
    attron(COLOR_PAIR(PAIR_STATUS_BAR));
    
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    int pos = cursor.x - editor.margin + cursor.x_offset;

    char path[64];
    char cursor_info[64];

    sprintf(path, " %s%s", get_filename(editor.filename),
            editor.is_saved ? "" : " [+]");
    sprintf(cursor_info, "Ln %d, Col %d", cursor.y + cursor.y_offset + 1,
            pos + 1);

    int space_length = max_x - strlen(path) - strlen(cursor_info);

    if (space_length < 0) {
        space_length = 0;
    }

    char *padding = mult_char(' ', space_length);
    mvprintw(max_y - 2, 0, "%s%s%s", path, padding, cursor_info);
    free(padding);

    attroff(COLOR_PAIR(PAIR_STATUS_BAR));
}

void draw_bottom_text(void) {
    int screen_height = getmaxy(stdscr);
    mvprintw(screen_height - 1, 0, "%s", editor.bottom_text);
}

void place_cursor(void) {
    char *line = editor.lines[cursor.y + cursor.y_offset];
    int render_x = calc_render_x(line, cursor.x + cursor.x_offset - editor.margin);
    move(cursor.y, editor.margin + render_x - cursor.x_offset);
}
