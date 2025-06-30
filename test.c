#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_lines(char** lines, int length) {
  for (int i = 0; i < length; i++) {
    printf("%s\n", lines[i]);
  }
}

int main() {
  char* lines[] = {strdup("Hello"), strdup("This"), strdup("Fuck")};
  int length = sizeof(lines) / sizeof(lines[0]);

  printf("Before: \n");
  print_lines(lines, length);
  printf("\n");

  char* new_line = realloc(lines[0], strlen(lines[0]) + strlen(lines[1]) + 1);
  if (!new_line) {
    printf("realloc failed!");
    return 1;
  }

  lines[0] = new_line;

  strcat(lines[0], lines[1]);

  printf("After: \n");
  print_lines(lines, length);
  
  return 0;
}
