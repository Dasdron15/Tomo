#ifndef COMMAND_PALETTE_H
#define COMMAND_PALETTE_H

enum Action {
    CHANGE_THEME,
    GO_TO_LINE,
    SAVE_FILE,
    OPEN_FILE,
    UNDO,
    REDO,
    EXIT,
};

int draw_command_palette(void);
char *draw_theme_palette(void);

#endif
