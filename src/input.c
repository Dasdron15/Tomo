#include "input.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "command_palette.h"
#include "cursor.h"
#include "edit.h"
#include "editor.h"
#include "fileio.h"
#include "select.h"
#include "themes.h"
#include "undo.h"

static bool save_undo_snapshot = true;
static int same_key = 0;

void handle_key(int key) {
    editor.bottom_text = "";

    if (key == KEY_UP) {
        move_up(false);
        save_undo_snapshot = true;
    }

    if (key == KEY_DOWN) {
        move_down(false);
        save_undo_snapshot = true;
    }

    if (key == KEY_RIGHT) {
        move_right(false);
        save_undo_snapshot = true;
    }

    if (key == KEY_LEFT) {
        move_left(false);
        save_undo_snapshot = true;
    }

    if (key == 402) { // Shift + RIGHT_ARROW (Right arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_right(true);
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == 393) { // Shift + LEFT_ARROW (Left arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_left(true);
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == 337) { // Shift + UP_ARROW (Up arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_up(true);
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == 336) { // Shift + DOWN_ARROW (Down arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_down(true);
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == 555) { // Shift + ctrl + LEFT_ARROW
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        cursor.x = editor.margin;
        cursor.x_offset = 0;
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == 570) { // Shift + ctrl + LEFT_ARROW
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        int line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
        cursor.x = editor.margin + line_len - cursor.x_offset;
        clamp_cursor();
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == 576) { // Shift + ctrl + UP_ARROW
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        cursor.y_offset = 0;
        cursor.y = 0;
        clamp_cursor();
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    // CTRL + L (SELECT A SINGLE LINE)
    if (key == 12) {
        start_selection(cursor.y + cursor.y_offset, 0);
        int line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
        cursor.x = editor.margin + line_len - cursor.x_offset;
        clamp_cursor();
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }
    if (key == 535 &&
        cursor.y + cursor.y_offset <
            editor.total_lines +
                1) { // CTRL + DOWN_ARROW (Jump to the end of the file)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        if (editor.total_lines > getmaxy(stdscr) - 2) {
        cursor.y_offset = editor.total_lines - getmaxy(stdscr) + 2;
        }
        cursor.y = editor.total_lines - cursor.y_offset - 1;
        clamp_cursor();
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == 26) { // CTRL + Z (Undo)
        undo();
        clamp_cursor();
    }

    if (key == 18) { // CTRL + R (Redo)
        redo();
        clamp_cursor();
    }

    if (key == 27) { // Esc
        cancel_selection();
    }

    if (key == 16) {
        int command = draw_command_palette();

        switch (command) {
        case CHANGE_THEME: {
            char *selected_theme = draw_theme_palette();
            load_theme(selected_theme);
            break;
        }
        case GO_TO_LINE:
            handle_key(7); // 7 - Ctrl + g
            break;

        case SAVE_FILE:
            save_file();
            break;

        case OPEN_FILE:
            if (!is_saved()) {
                ask_for_save(false);
            }

            open_dir(dirname(editor.filename));
            break;

        case UNDO:
            undo();
            break;

        case EXIT:
            exit_editor();
            break;
        }
    }

    if (key == 20) {
        char *selected_theme = draw_theme_palette();
        load_theme(selected_theme);
    }

    if (key == 17) { // CTRL + Q (Quit the editor)
        exit_editor();
    }

    if (key >= 32 && key <= 126 && !is_selecting()) { // ASCII symbols
        if (same_key >= 32 && same_key <= 126) {
            save_undo_snapshot = false;
        } else if (strchr(" (\"'", key) != NULL) {
            save_undo_snapshot = true;
        } else {
            save_undo_snapshot = true;
        }

        take_snapshot(save_undo_snapshot);
        insert_char((char)key);
    }

    if (key >= 32 && key <= 126 && is_selecting()) {
        Point start_select;
        Point end_select;

        get_selection_bounds(&start_select, &end_select);
        deletion(start_select, end_select);
        cancel_selection();
        insert_char((char)key);
    }

    if (key == '\t') {
        take_snapshot(true);
        add_tab();
        cursor.max_x = cursor.x + cursor.x_offset;
    }

    if (key == KEY_BACKSPACE || key == 127) {
        if (same_key == KEY_BACKSPACE || same_key == 127) {
            save_undo_snapshot = false;
        } else {
            save_undo_snapshot = true;
        }

        Point start_select;
        Point end_select;

        get_selection_bounds(&start_select, &end_select);

        take_snapshot(save_undo_snapshot);

        deletion(start_select, end_select);
    }

    if (key == KEY_ENTER || key == 10) {
        take_snapshot(true);
        new_line();
    }

    if (key == 19) { // CTRL + S (Save file)
        save_file();
    }

    if (key == 7) { // CTRL + G (Goto line)
        int target = goto_line();
        if (target != -1) {
        cursor.y = target - cursor.y_offset;
        clamp_cursor();
        }
    }

    if (key == 5) { // Ctrl + E (Open file)
        if (!is_saved()) {
            ask_for_save(false);
        }

        open_dir(dirname(editor.filename));
    }

    if (key == 534 &&
        cursor.y + cursor.y_offset <
            editor.total_lines +
                1) { // CTRL + DOWN_ARROW (Jump to the end of the file)
        if (editor.total_lines > getmaxy(stdscr) - 2) {
        cursor.y_offset = editor.total_lines - getmaxy(stdscr) + 2;
        }
        cursor.y = editor.total_lines - cursor.y_offset - 1;
        clamp_cursor();
    }

    if (key == 575) { // CTRL + UP_ARROW (Jump to the beggining of the file)
        cursor.y_offset = 0;
        cursor.y = 0;
        clamp_cursor();
    }

    if (key == 554) { // CTRL + RIGHT_ARROW (Jump to the beggining of the line)
        cursor.x = editor.margin;
        cursor.x_offset = 0;
    }

    if (key == 569) { // CTRL + LEFT_ARROW (Jump to the end of the line)
        int line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
        cursor.x = editor.margin + line_len - cursor.x_offset;
        clamp_cursor();
    }

    if (key == 3 && is_selecting()) { // CTRL + C (Copy)
        Point start_select;
        Point end_select;

        get_selection_bounds(&start_select, &end_select);
        copy_text(start_select, end_select);

        editor.bottom_text = "Selection copied";
    }

    if (key == 24 && is_selecting()) { // CTRL + X (Cut)
        Point start_select;
        Point end_select;

        get_selection_bounds(&start_select, &end_select);
        copy_text(start_select, end_select);
        deletion(start_select, end_select);

        editor.bottom_text = "Cut selection";
    }

    if (key == 22) { // CTRL + V (Paste)
        Point start_select;
        Point end_select;

        take_snapshot(true);
        if (is_selecting()) {
            get_selection_bounds(&start_select, &end_select);
            deletion(start_select, end_select);
            cancel_selection();
        }

        paste_text();

        editor.bottom_text = "Clipboard pasted";
    }

    same_key = key;
}
