// ------------- Task - 1 ------------- 

#include<stdio.h>
#include<fcntl.h>


int main() {

    int fd;
    char buffer[80];
    static char message[] = "Hello, world";
    char inp[50];
    char x [] = "";

    // scanf("Please type something: ", &n);
    
    fd = open("myfile",O_RDWR);
    
    while (1 == 1){
        // scan for the input string (scan for the input commands)
        if (x == "-1"){
            //scan for the string to input
            // and write
            if (fd != -1) {
                printf("myfile opened for read/write access\n");
                write(fd, message, sizeof(message));
                lseek(fd, 0, SEEK_CUR); /* go back to the beginning of the file */
                // read(fd, buffer, sizeof(message));
                printf(" %s was written to myfile \n", buffer);
                close (fd);
            }
        }
        else {
            break;
        }

    }
}