#ifndef FILE_TREE_H
#define FILE_TREE_H

#include <stdbool.h>

typedef struct Node {
    char *name;
    char *path;
    bool is_dir;
    bool expanded;
    int depth;

    struct Node **children;
    int child_count;

    bool loaded;
    struct Node *parent;
} Node;

Node *node_create(const char *path, struct Node *parent, int depth);
void load_children(Node *node);

#endif