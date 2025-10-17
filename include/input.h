#ifndef INPUT_H
#define INPUT_H

#define MOVE_UP -1
#define MOVE_DOWN 1

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

void define_esc_arrows(void);
void handle_key(int key);

#endif
