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

#define CTRL(c) ((c) & 037)

// Shift + arrow (left and right keys are defined by default in ncurses)
#define KEY_SUP   5601
#define KEY_SDOWN 5602

// Ctrl + arrow
#define KEY_CLEFT  5701
#define KEY_CRIGHT 5702
#define KEY_CUP    5703
#define KEY_CDOWN  5704

// Alt + arrow
#define KEY_ALEFT  5801
#define KEY_ARIGHT 5802
#define KEY_AUP    5803
#define KEY_ADOWN  5804

#define KEY_TAB '\t'
#define KEY_DELETE 127
#define KEY_ESCAPE 27
#define KEY_LINEFEED 10

static bool save_undo_snapshot = true;
static int same_key = 0;

/*
 * Because keycodes for modifier + arrow keys may vary in different terminals,
 * these keys should be defined as escape keys, for better portability.
 */
void define_esc_arrows(void) {
    define_key("\033[1;2A", KEY_SUP);
    define_key("\033[1;2B", KEY_SDOWN);
    define_key("\033[1;2C", KEY_SRIGHT);
    define_key("\033[1;2D", KEY_SLEFT);

    define_key("\033[1;5A", KEY_CUP);
    define_key("\033[1;5B", KEY_CDOWN);
    define_key("\033[1;5C", KEY_CRIGHT);
    define_key("\033[1;5D", KEY_CLEFT);

    define_key("\033[1;3A", KEY_AUP);
    define_key("\033[1;3B", KEY_ADOWN);
    define_key("\033[1;3C", KEY_ARIGHT);
    define_key("\033[1;3D", KEY_ALEFT);
}

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

    if (key == KEY_CUP) { // (Jump to the beggining of the file)
        cursor.y_offset = 0;
        cursor.y = 0;
        clamp_cursor();

        if (is_selecting()) {
            update_selection(cursor.y + cursor.y_offset,
                             cursor.x - editor.margin + cursor.x_offset);
        }
    }
    
    if (key == KEY_CDOWN) { // (Jump to the end of the file)
        cursor.y_offset = 0;
        cursor.y = editor.total_lines;
        clamp_cursor();

        if (is_selecting()) {
            update_selection(cursor.y + cursor.y_offset,
                             cursor.x - editor.margin + cursor.x_offset);
        }
    }

    if (key == KEY_CRIGHT) { // (Jump to the end of the line)
        int line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
        cursor.x = editor.margin + line_len - cursor.x_offset;
        clamp_cursor();

        if (is_selecting()) {
            update_selection(cursor.y + cursor.y_offset,
                             cursor.x - editor.margin + cursor.x_offset);
        }
    }

    if (key == KEY_CLEFT) { // (Jump to the beggining of the line)
        cursor.x = editor.margin;
        cursor.x_offset = 0;
        clamp_cursor();

        if (is_selecting()) {
            update_selection(cursor.y + cursor.y_offset,
                             cursor.x - editor.margin + cursor.x_offset);
        }
    }

    if (key == KEY_SUP) { // (Up arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_up(true);
        update_selection(cursor.y + cursor.y_offset,
                         cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == KEY_SDOWN) { // (Down arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_down(true);
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == KEY_SRIGHT) { // (Right arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_right(true);
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == KEY_SLEFT) { // (Left arrow selection)
        start_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        move_left(true);
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == CTRL('l')) { // (Select a single line)
        start_selection(cursor.y + cursor.y_offset, 0);
        int line_len = strlen(editor.lines[cursor.y + cursor.y_offset]);
        cursor.x = editor.margin + line_len - cursor.x_offset;
        clamp_cursor();
        update_selection(cursor.y + cursor.y_offset,
                        cursor.x - editor.margin + cursor.x_offset);
        save_undo_snapshot = true;
    }

    if (key == CTRL('z')) { // (Undo)
        undo();
        clamp_cursor();
    }

    if (key == CTRL('r')) { // (Redo)
        redo();
        clamp_cursor();
    }

    if (key == KEY_ESCAPE) {
        cancel_selection();
    }

    if (key == CTRL('p')) {
        int command = draw_command_palette();

        switch (command) {
        case CHANGE_THEME: {
            char *selected_theme = draw_theme_palette();
            load_theme(selected_theme);
            break;
        }
        case GO_TO_LINE:
            handle_key(CTRL('g'));
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

        case REDO:
            redo();
            break;

        case EXIT:
            exit_editor();
            break;
        }
    }

    if (key == CTRL('t')) {
        char *selected_theme = draw_theme_palette();
        load_theme(selected_theme);
    }

    if (key == CTRL('q')) { // (Quit the editor)
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

    if (key == KEY_TAB) {
        take_snapshot(true);
        add_tab();
        cursor.max_x = cursor.x + cursor.x_offset;
    }
    
    if (key == KEY_BACKSPACE || key == KEY_DELETE) {
        if (same_key == KEY_BACKSPACE || same_key == KEY_DELETE) {
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

    if (key == KEY_ENTER || key == KEY_LINEFEED) {
        take_snapshot(true);
        new_line();
    }

    if (key == CTRL('s')) { // (Save file)
        save_file();
    }

    if (key == CTRL('g')) { // (Goto line)
        int target = goto_line();
        if (target != -1) {
        cursor.y = target - cursor.y_offset;
        clamp_cursor();
        }
    }

    if (key == CTRL('e')) { // (Open file)
        if (!is_saved()) {
            ask_for_save(false);
        }

        open_dir(dirname(editor.filename));
    }

    if (key == CTRL('c') && is_selecting()) { // (Copy)
        Point start_select;
        Point end_select;

        get_selection_bounds(&start_select, &end_select);
        copy_text(start_select, end_select);

        editor.bottom_text = "Selection copied";
    }

    if (key == CTRL('x') && is_selecting()) { // (Cut)
        Point start_select;
        Point end_select;

        get_selection_bounds(&start_select, &end_select);
        copy_text(start_select, end_select);
        deletion(start_select, end_select);

        editor.bottom_text = "Cut selection";
    }

    if (key == CTRL('v')) { // (Paste)
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
