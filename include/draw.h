#ifndef DRAW_H
#define DRAW_H

#include <stdbool.h>

int calc_render_x(const char *line, int cursor_x);
void draw_text_area(void);
void draw_line_with_number(int index, int y_pos);
void draw_line_content(int index, char *line, int y);
void draw_command_palette(void);
void draw_status_bar(void);
void draw_bottom_text(void);
void place_cursor(void);

#endif
