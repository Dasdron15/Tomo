#include "fileio.h"

void load_file(const char* path, Text_Buffer *buffer) {
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        perror("Error opening file");
        endwin();
        exit(0);
    }

    size_t len = 0;
    while (fgets(buffer->Text, sizeof(buffer->Text), fp)) {
        len += strlen(buffer->Text) + len;
    }

    fclose(fp);
}

void save_file(const char* path, const Text_Buffer *buffer) {
    FILE *fp = fopen(path, "w");

    if (fp == NULL) {
        perror("Error writing to a file");
        return;
    }

    fprintf(fp, "%s", buffer->Text);
    fclose(fp);
}
