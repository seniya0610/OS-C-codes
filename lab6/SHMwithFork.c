#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    const int SIZE = 4096;
    const char *name = "OS";
    const char *msg0 = "Hello";
    const char *msg1 = "World!";

    int fd;
    char *ptr;
    pid_t pid;

    // create shared memory BEFORE fork so both processes can access it
    fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    ptr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    pid = fork();

    if (pid > 0) {                          // PARENT = producer
        sprintf(ptr, "%s", msg0);           // write "Hello"
        ptr += strlen(msg0);
        sprintf(ptr, "%s", msg1);           // write "World!"
        wait(NULL);                         // wait for child to finish
        shm_unlink(name);                   // clean up shared memory
    }
    else {                                  // CHILD = consumer
        sleep(1);                           // wait a moment so parent writes first
        printf("Consumer read: %s\n", (char *)ptr);   // read and print
    }

    return 0;
}
