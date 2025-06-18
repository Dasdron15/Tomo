#include "../utils/common.h"
#include "status_bar.h"
#include "../editor.h"

void draw_status_bar(struct Editor_State* state) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    int margin = int_len(state->total_lines) + 2;
    int pos = state->cursor_x - margin;
    
    char path[64];
    char cursor_info[64];

    sprintf(path, " %s%s", get_filename(state->filename), state->is_saved ? "" : " [+]");
    sprintf(cursor_info, "Ln %d, Col %d", state->cursor_y + state->y_offset + 1, pos + 1);

    int space_length = max_x - strlen(path) - strlen(cursor_info);

    if (space_length < 0) {
        space_length = 0;
    }

    char* padding = mult_char(' ', space_length);
    mvprintw(max_y - 1, 0, "%s%s%s", path, padding, cursor_info);
    free(padding);
}
