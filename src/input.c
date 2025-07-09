#include "input.h"

#include <curses.h>
#include <stdlib.h>

#include "cursor.h"
#include "select.h"
#include "editor.h"
#include "edit.h"
#include "fileio.h"

void handle_key(int key) {
    if (key == KEY_UP) {
        move_up(false);
        return;
    }

    if (key == KEY_DOWN) {
        move_down(false);
        return;
    }

    if (key == KEY_RIGHT) {
        move_right(false);
        return;
    }

    if (key == KEY_LEFT) {
        move_left(false);
        return;
    }

    if (key == 402) { // Shift + RIGHT_ARROW (Right arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_right(true);
        update_selection(cursor.y + cursor.y_offset,
                         cursor.x - editor.margin + cursor.x_offset);
        return;
    }

    if (key == 393) { // Shift + LEFT_ARROW (Left arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_left(true);
        update_selection(cursor.y + cursor.y_offset,
                         cursor.x - editor.margin + cursor.x_offset);
        return;
    }

    if (key == 337) { // Shift + UP_ARROW (Up arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_up(true);
        update_selection(cursor.y + cursor.y_offset,
                         cursor.x - editor.margin + cursor.x_offset);
        return;
    }

    if (key == 336) { // Shift + DOWN_ARROW (Down arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_down(true);
        update_selection(cursor.y + cursor.y_offset,
                         cursor.x - editor.margin + cursor.x_offset);
        return;
    }

    if (key == 27) { // Esc
        cancel_selection();
        return;
    }

    if (key == 17) { // CTRL + Q (Quit the editor)
        if (!editor.is_saved) {
            ask_for_save();
        } else {
            reset();
            endwin();
            exit(0);
        }
        return;
    }

    if ((key >= 32 && key <= 126)) {
        Point start_select;
        Point end_select;
        
        if (is_selecting()) {
            get_selection_bounds(&start_select, &end_select);
            deletion(start_select, end_select);
            cancel_selection();
        }
        
        insert_char((char) key);
        editor.is_saved = false;
        return;
    }

    if (key == '\t') {
        add_tab();
        cursor.max_x = cursor.x;
        editor.is_saved = false;
        return;
    }

    if (key == KEY_BACKSPACE || key == 127) {
        Point start_select;
        Point end_select;

        get_selection_bounds(&start_select, &end_select);
        
        deletion(start_select, end_select);
        editor.is_saved = false;
        return;
    }

    if (key == KEY_ENTER || key == 10) {
        new_line();
        editor.is_saved = false;
        return;
    }

    if (key == 19) { // CTRL + S (Save file)
        save_file();
        return;
    }

    if (key == 7) { // CTRL + G (Goto line)
        int target = goto_line();
        if (target != -1) {
            cursor.y = target - cursor.y_offset;
            clamp_cursor();
        }
        return;
    }

    if (key == 534 &&
        cursor.y + cursor.y_offset <
            editor.total_lines +
                1) { // CTRL + DOWN_ARROW (Jump to the end of the file)
        if (editor.total_lines > getmaxy(stdscr) - 1) {
            cursor.y_offset = editor.total_lines - getmaxy(stdscr) + 1;
        }
        cursor.y = editor.total_lines - cursor.y_offset - 1;
        clamp_cursor();
        return;
    }

    if (key == 575) { // CTRL + UP_ARROW (Jump to the beggining of the file)
        cursor.y_offset = 0;
        cursor.y = 0;
        clamp_cursor();
        return;
    }

    if (key == 3 && is_selecting()) { // CTRL + C (Copy)
        Point start_select;
        Point end_select;
        
        get_selection_bounds(&start_select, &end_select);
        copy_text(start_select, end_select);
        return;
    }

    if (key == 24 && is_selecting()) { // CTRL + X (Cut)
        Point start_select;
        Point end_select;
        
        get_selection_bounds(&start_select, &end_select);        
        
        copy_text(start_select, end_select);
        deletion(start_select, end_select);

        editor.is_saved = false;
        return;
    }

    if (key == 22) {
        paste_text();
        return;                
    }
}

