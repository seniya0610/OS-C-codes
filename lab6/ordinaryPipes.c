//using fork
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define READ_END  0
#define WRITE_END 1
#define SIZE 25

int main() {
    int fd[2];           // fd[0] = read end, fd[1] = write end
    char write_msg[SIZE] = "Greetings";
    char read_msg[SIZE];
    pid_t pid;

    pipe(fd);            // create the pipe (MUST be before fork)
    pid = fork();        // create child process

    if (pid > 0) {                                    // PARENT = writer
        close(fd[READ_END]);                          // parent won't read, close it
        write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);
        close(fd[WRITE_END]);                         // done writing
    }
    else {                                            // CHILD = reader
        close(fd[WRITE_END]);                         // child won't write, close it
        read(fd[READ_END], read_msg, SIZE);
        printf("Child read: %s\n", read_msg);
        close(fd[READ_END]);
    }

    return 0;
}
