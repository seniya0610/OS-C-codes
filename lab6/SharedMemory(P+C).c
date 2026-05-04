//producer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    const int SIZE = 4096;
    const char *name = "OS";              // name of shared memory object
    const char *msg0 = "Hello";
    const char *msg1 = "World!";

    int fd;
    char *ptr;

    fd = shm_open(name, O_CREAT | O_RDWR, 0666);  // create shared memory object
    ftruncate(fd, SIZE);                            // set its size

    // map it into this process's address space (ptr now points to shared memory)
    ptr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sprintf(ptr, "%s", msg0);           // write "Hello" to shared memory
    ptr += strlen(msg0);
    sprintf(ptr, "%s", msg1);           // write "World!" right after it

    return 0;
}

//consumer
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>

int main() {
    const int SIZE = 4096;
    const char *name = "OS";

    int fd;
    char *ptr;

    fd = shm_open(name, O_RDONLY, 0666);          // open same object, read only
    // map it into this process's address space
    ptr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    printf("%s\n", (char *)ptr);                  // print what producer wrote
    shm_unlink(name);                             // delete the shared memory object
    return 0;
}
