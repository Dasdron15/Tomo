#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cursor.h"
#include "editor.h"
#include "select.h"
#include "undo.h"

#define UNDO_DEPTH 64

typedef struct {
    char **lines;
    Point cursor_pos;
    Point selection_start;
    Point selection_end;
} Snapshot;

typedef struct {
    Snapshot *items;
    int top;
    int capacity;
} SnapshotStack;

/**
 * Creates a new snapshot of the current editor state:
 * Cursor position, text lines, etc.
 */
Snapshot create_snapshot() {
    Snapshot copy;
    copy.cursor_pos = (Point){cursor.y, cursor.x};
    copy.selection_start = (Point){-1, -1};
    copy.selection_end = (Point){-1, -1};

    if (is_selecting()) {
        copy.selection_start = get_selection_start();
        copy.selection_end = get_selection_end();
    }

    copy.lines = malloc(sizeof(char *) * editor.total_lines);

    return copy;
}

/**
 * Initializes an empty snapshot stack with given capacity.
 */
SnapshotStack *create_stack(int capacity) {
    SnapshotStack *stack = malloc(sizeof(SnapshotStack));
    stack->items = malloc(sizeof(Snapshot) * capacity);
    stack->top = -1;
    stack->capacity = capacity;
    return stack;
}

/* is_empty checks if provided stack is empty */
bool is_empty(SnapshotStack *stack) {
    return stack->top == -1; 
}

/* is_full checks if provided stack is full */
bool is_full(SnapshotStack *stack) {
    return stack->top == stack->capacity - 1;
}

/* push function pushes the probided value to the top of the stack */
void push(SnapshotStack *stack, Snapshot value) {
    // Check if there is not enough space in the stack
    if (is_full(stack)) {
        stack->capacity *= 2;
        stack->items = realloc(stack->items, sizeof(Snapshot) * stack->capacity);
    }
    stack->items[++stack->top] = value;
}

/* Delete the last element in the stack */
Snapshot pop(SnapshotStack *stack) {
    // Throw an error if the stack is empty
    if (is_empty(stack)) {
        endwin();
        reset();
        fprintf(stderr, "Stack is empty\n");
        exit(1);
    }
    return stack->items[stack->top--];
}

/* Returns the top value of the stack */
Snapshot peek(SnapshotStack *stack) {
    // Throw an error if the stack is empty
    if (is_empty(stack)) {
        endwin();
        reset();
        fprintf(stderr, "Stack is empty\n");
        exit(1);
    }
    return stack->items[stack->top];
}

void take_snapshot(bool merge_with_previous) {

}

void undo(void) {

}
