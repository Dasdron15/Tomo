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
    Point offset;
    bool is_selecting;
    Point selection_start;
    Point selection_end;
} Snapshot;

typedef struct {
    Snapshot *items;
    int top;
    int capacity;
} SnapshotStack;

SnapshotStack undo_stack;

void free_snapshot(Snapshot *snap) {
    if (!snap) return;
    if (snap->lines) {
        for (int i = 0; i < snap->total_lines; i++) {
            free(snap->lines[i]);
        }
        free(snap->lines);
        snap->lines = NULL;
    }
    snap->total_lines = 0;
    snap->is_selecting = false;
}

/*
 * Creates a new snapshot of the current editor state:
 * Cursor position, text lines, etc.
 */
Snapshot create_snapshot(void) {
    Snapshot copy;
    copy.cursor_pos = (Point){cursor.y, cursor.x};
    copy.offset = (Point){cursor.y_offset, cursor.x_offset};
    copy.selection_start = (Point){-1, -1};
    copy.selection_end = (Point){-1, -1};

    copy.is_selecting = is_selecting();

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
    stack->items = calloc(capacity, sizeof(Snapshot));
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
    if (is_full(stack)) {
        free_snapshot(&stack->items[0]);

        memmove(&stack->items[0], &stack->items[1], sizeof(Snapshot) * (stack->capacity - 1));
        stack->top = stack->capacity - 2;
    }
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

/*
 * Create a new Snapshot object with current editor state
 * and push newly created object to the top of the stack
 */
void take_snapshot(bool need_save) {
    if (!need_save) return;   
    Snapshot snapshot = create_snapshot();
    push(&undo_stack, snapshot);
}

/*
 * Check if the undo stack is empty if not then 
 * take the top object from undo stack, assign
 * all the values from the object to the current editor state,
 * resize the main lines array and remove the top object 
 * from the stack
 */
void undo(void) {
    if (is_empty(&undo_stack)) return;
    
    Snapshot snap = pop(&undo_stack);

    if (editor.lines) {
        for (int i = 0; i < editor.total_lines; i++) {
            free(editor.lines[i]);
        }
        free(editor.lines);
        editor.lines = NULL;
    }

    editor.lines = malloc(sizeof(char*) * snap.total_lines);
    for (int i = 0; i < snap.total_lines; i++) {
        editor.lines[i] = strdup(snap.lines[i]);
    }
    editor.total_lines = snap.total_lines;

    cursor.x = snap.cursor_pos.x - (cursor.x_offset - snap.offset.x);
    cursor.y = snap.cursor_pos.y - (cursor.y_offset - snap.offset.y);
    if (snap.is_selecting) {
        set_selection(snap.selection_start, snap.selection_end);
    }

    free_snapshot(&snap);
}
