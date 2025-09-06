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
    int total_lines;
    Point cursor_pos;
    Point selection_start;
    Point selection_end;
} Snapshot;

typedef struct {
    Snapshot *items;
    int top;
    int capacity;
} SnapshotStack;

SnapshotStack undo_stack;

/*
 * Creates a new snapshot of the current editor state:
 * Cursor position, text lines, etc.
 */
Snapshot create_snapshot(void) {
    Snapshot copy;
    copy.cursor_pos = (Point){cursor.y, cursor.x};
    copy.selection_start = (Point){-1, -1};
    copy.selection_end = (Point){-1, -1};

    if (is_selecting()) {
        copy.selection_start = get_selection_start();
        copy.selection_end = get_selection_end();
    }

    copy.lines = malloc(sizeof(char*) * editor.total_lines);
    for (int i = 0; i < editor.total_lines; i++) {
        copy.lines[i] = strdup(editor.lines[i]);
    }

    copy.total_lines = editor.total_lines;
    return copy;
}

/*
 * Initializes an empty snapshot stack with given capacity.
 */
void create_stack(SnapshotStack *stack, int capacity) {
    stack->items = malloc(sizeof(Snapshot) * capacity);
    stack->top = -1;
    stack->capacity = capacity;
}

/*
 * Returns true if the stack has no snapshots
 */
bool is_empty(SnapshotStack *stack) {
    return stack->top == -1; 
}

/*
 * Returns true if the stack is full
 */
bool is_full(SnapshotStack *stack) {
    return stack->top == stack->capacity - 1;
}

/*
 * Pushes a snapshot onto the stack
 */
void push(SnapshotStack *stack, Snapshot value) {
    stack->items[++stack->top] = value;
}

/*
 * Removes and returns the top snapshot
 */
Snapshot pop(SnapshotStack *stack) {
    return stack->items[stack->top--];
}

/*
 * Returns the top snapshot
 */
Snapshot peek(SnapshotStack *stack) {
    return stack->items[stack->top];
}

void init_undo_stack(void) {
    create_stack(&undo_stack, UNDO_DEPTH);
}

void take_snapshot(void) {
    Snapshot snapshot = create_snapshot();

    push(&undo_stack, snapshot);
}

void undo(void) {
    if (!is_empty(&undo_stack)) {
        Snapshot top_snapshot = peek(&undo_stack);

        cursor.x = top_snapshot.cursor_pos.x;
        cursor.y = top_snapshot.cursor_pos.y;

        if (is_selecting()) {
            
        }

        for (int i = 0; i < top_snapshot.total_lines; i++) {
            editor.lines[i] = strdup(top_snapshot.lines[i]);
        }
        pop(&undo_stack);
    }
}
