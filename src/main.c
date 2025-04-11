#include <curses.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    FILE* fp = fopen(argv[1], "r");
    char code[sizeof(fp)];
    initscr();
    noecho();
    curs_set(1);

    if (fp == NULL) {
        printw("File cannot be opened\n");
        return 1;
    }

    while (1) {
        int ch = getch();
        if (ch == 27) {
            break;
        }
        
    }
    endwin();

    return 0;
}