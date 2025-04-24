#include "editor.h"
#include "utils/fileio.h"
#include <curses.h>
#include <pthread.h>
#include <unistd.h>

struct Editor_State* state;

// void* draw_loop(void* arg) {
//     const unsigned int fps = 60;
//     const useconds_t delay = 1000000 / fps;

//     while (1) {
//         draw_editor(state);
//         usleep(delay);
//     }
//     return NULL;
// }

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    state = malloc(sizeof(struct Editor_State));
    state->filename = argv[1];

    load_file(argv[1], state);
    init_editor(state);
    
    // pthread_t draw_thread;
    // pthread_create(&draw_thread, NULL, draw_loop, NULL);

    while (1) {
        draw_editor(state);
        int key = getch();
        move_cursor(key, state);
        handle_key(key, state);
    }

    endwin();
    return 0;
}
