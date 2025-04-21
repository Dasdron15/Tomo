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
    char display_time[64];

    time_t sec = time(NULL);
    struct tm* current_time = localtime(&sec);

    sprintf(path, "%s%s", get_filename(state->filename), state->is_saved ? "" : "*");
    sprintf(cursor_info, "Ln %d, Col %d", state->cursor_y + state->scroll_offset + 1, pos + 1);
    sprintf(display_time, "%02d:%02d:%02d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    int space_length = max_x - strlen(path) - 2 - strlen(cursor_info) - strlen(display_time);

    if (space_length < 0) {
        space_length = 0;
    }

    char* padding = mult_char(' ', space_length);
    mvprintw(max_y - 1, 0, "%s%s%s  %s", path, padding, cursor_info, display_time);
    free(padding);
}
