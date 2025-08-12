#include <stdio.h>
#include <dirent.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Error: not enough arguments\n");
        return 1;
    }
    
    DIR *d;
    struct dirent *dir;

    d = opendir(argv[1]);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                printf("%s\n", dir->d_name);
            }
        }
        closedir(d);
    }

    return 0;
}

// #include <stdio.h>

// int main(int argc, char **argv) {
//     FILE *fp = fopen(argv[1], "r");

//     if (!fp) {
//         perror("Can't open the file");
//         return 1;
//     }

//     char ch;
//     while ((ch = fgetc(fp)) != EOF) {
//         putchar(ch);
//     }

//     fclose(fp);
//     return 0;
// }
