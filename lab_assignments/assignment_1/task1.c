// --------------- Task - 1 -------------------------
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char str[100], str1[100];

    fd = open("anyfile.txt", O_RDWR | O_CREAT, 0666);
    printf("anyfile.txt opened for read/write access\n");

    while (1) {
        printf("Type '-1' to write something");
        scanf("%s", str);

        if (strcmp(str, "-1") == 0) {
            printf("Please enter a string to write: ");
            scanf("%s", str1);

            write(fd, str1, strlen(str1));
            lseek(fd, 3, SEEK_CUR); // Skip 3 bytes in file
            printf("\"%s\" was written to anyfile.txt\n", str1);
        } else {
            close(fd);
            printf("File closed.\n");
            break;
        }
    }

    return 0;
}
