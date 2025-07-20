#ifndef DRAW_H
#define DRAW_H

#include <stdbool.h>

int calc_render_x(const char *line, int cursor_x);
void draw_text_area(void);
void draw_line_with_number(int index, int y_pos);
void draw_line_content(int index, char *line, int y);
int syntax_color(char *line, int pos);
bool is_keyword(char *string, const char *keyword);
void draw_bottom_text(void);
void place_cursor(void);

#endif
