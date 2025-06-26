#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

void remove_range(char* line[], int length, Point start, Point end) {
    if (start.y < 0 || end.y >= length || start.y > end.y) return;

    int remove_count = end.y - start.y + 1;

    for (int i = end.y + 1; i < length; i++) {
        line[i - remove_count] = line[i];    
    }

    for (int i = length - remove_count; i < length; i++) {
        line[i] = NULL;
    }
}


int main(void) {
    char *str[4] = {"Hey,", "I", "Love", "You)"};
    int length = sizeof(str) / sizeof(str[0]);

    Point start_point = {0, 0};
    Point end_point = {1, 1};

    printf("Before removal:\n");
    for (int i = 0; i < length; i++) {
        printf("%s\n", str[i]);
    }
    
    printf("\n");
    
    remove_range(str, length, start_point, end_point);

    printf("After removal:\n");
    for (int i = 0; i < length && str[i] != NULL; i++) {
        printf("%s\n", str[i]);
    }
    
    return 0;
}
