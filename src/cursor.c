#include "cursor.h"

#include <ctype.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"
#include "select.h"
#include "utils.h"

CursorState cursor;

void move_up(bool is_selecting) {
  if (cursor.y + cursor.y_offset > 0) {
    cursor.y--;
    cursor.x = cursor.max_x;
  } else {
    cursor.x = editor.margin - cursor.x_offset;
    cursor.max_x = cursor.x;
  }

  if (!is_selecting) {
    cancel_selection();
  }

  clamp_cursor();
}

void move_down(bool is_selecting) {
  if (cursor.y + cursor.y_offset < editor.total_lines - 1) {
    cursor.y++;
    cursor.x = cursor.max_x;
  } else {
    size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
    cursor.x = line_len + editor.margin - cursor.x_offset;
    cursor.max_x = cursor.x;
  }

  if (!is_selecting) {
    cancel_selection();
  }

  clamp_cursor();
}

void move_right(bool is_selecting) {
  int line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);

  if (line_len <= cursor.x + cursor.x_offset - editor.margin &&
      cursor.y + cursor.y_offset < editor.total_lines - 1) {
    cursor.x_offset = 0;
    cursor.x = editor.margin;
    cursor.y++;
  } else {
    cursor.x++;
  }

  cursor.max_x = cursor.x + cursor.x_offset;

  if (!is_selecting) {
    cancel_selection();
  }

  clamp_cursor();
}

void move_left(bool is_selecting) {
  if (cursor.x + cursor.x_offset > editor.margin) {
    cursor.x--;
  } else if (cursor.y > 0) {
    cursor.y--;
    size_t line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
    editor.margin = int_len(editor.total_lines) + 2;
    cursor.x = line_len + editor.margin;
  }

  cursor.max_x = cursor.x + cursor.x_offset;

  if (!is_selecting) {
    cancel_selection();
  }

  clamp_cursor();
}

void clamp_cursor(void) {
  int screen_width = getmaxx(stdscr);
  int screen_height = getmaxy(stdscr);
  editor.margin = int_len(editor.total_lines) + 2;

  int x_limit = 5;
  int y_limit = 3;

  int line_index = cursor.y + cursor.y_offset;
  if (line_index >= editor.total_lines) {
    line_index = editor.total_lines - 1;
    cursor.y = line_index - cursor.y_offset;
    if (cursor.y < 0) {
      cursor.y = 0;
    }
  }

  int line_len = strlen(editor.lines[line_index]);

  // Clamp X axis to line length
  if (cursor.x + cursor.x_offset - editor.margin > line_len) {
    cursor.x = line_len + editor.margin - cursor.x_offset;
  }

  // Y axis limit (scroll up)
  if (cursor.y < y_limit && cursor.y_offset > 0) {
    int y_diff = y_limit - cursor.y;
    if (cursor.y_offset - y_diff < 0) {
      cursor.y_offset = 0;
      cursor.y = is_selecting() ? get_selection_start().y : 0;
    } else {
      cursor.y_offset -= y_diff;
      cursor.y = y_limit;
    }
  }

  // Y axis limit (scroll down)
  if (cursor.y > screen_height - y_limit - 3) {
    cursor.y_offset += cursor.y - (screen_height - y_limit - 3);
    cursor.y = screen_height - y_limit - 3;
  }

  // X axis limit (scroll left)
  int left_threshold = x_limit + editor.margin;
  if (cursor.x < left_threshold && cursor.x_offset > 0) {
    int x_diff = left_threshold - cursor.x;
    if (cursor.x_offset < x_diff) {
      cursor.x_offset = 0;
      cursor.x = is_selecting() ? get_selection_start().x : editor.margin;
    } else {
      cursor.x_offset -= x_diff;
      cursor.x = left_threshold;
    }
  }

  // X axis limit (scroll right)
  int right_threshold = screen_width - x_limit;
  if (cursor.x > right_threshold) {
    cursor.x_offset += cursor.x - right_threshold;
    cursor.x = right_threshold;
  }
}

size_t goto_line(void) {
  int screen_height = getmaxy(stdscr);
  char *prompt = "Go to line: ";

  move(getmaxy(stdscr) - 1, 0);
  clrtoeol();

  mvprintw(screen_height - 1, 0, "%s", prompt);

  curs_set(1);

  char input[16] = {0};
  int pos = 0;

  int ch;
  while ((ch = wgetch(stdscr)) != '\n') {
    if (ch == 17) {
      if (!is_saved()) {
        ask_for_save();
      }
      endwin();
      reset();
      exit(0);
      return 0;
    }

    if (ch == 27) {
      return -1;
    }

    if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') && pos > 0) {
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

  int line = atoi(input);
  if (line < 1) {
    return -1;
  }

  if (line > editor.total_lines) {
    return editor.total_lines - 1;
  }

  return line - 1;
}
