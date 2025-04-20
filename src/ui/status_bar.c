#include "status_bar.h"
#include "../editor.h"

void draw_status_bar(struct Editor_State* state) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    time_t sec, val = 0;
    struct tm* current_time;

    sec = time(NULL);

    current_time = localtime(&sec);

    mvprintw(max_y - 1, 0, "%02d:%02d:%02d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
}
