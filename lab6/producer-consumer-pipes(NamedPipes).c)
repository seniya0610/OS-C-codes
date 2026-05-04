//producer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_FILE "/tmp/myfifo"
#define BUFSIZ 512

int main() {
    int fd;
    char buffer[BUFSIZ];
    ssize_t num_bytes;

    mkfifo(FIFO_FILE, 0666);               // create the named pipe file
    fd = open(FIFO_FILE, O_WRONLY);        // open for writing
    if (fd == -1) { perror("open"); exit(1); }

    while (1) {
        printf("Producer: Enter a message (or 'exit' to quit): ");
        fgets(buffer, BUFSIZ, stdin);                      // get input
        num_bytes = write(fd, buffer, strlen(buffer));     // send through pipe
        if (num_bytes == -1) { perror("write"); exit(1); }
        if (strncmp(buffer, "exit", 4) == 0) break;       // quit on "exit"
    }

    close(fd);
    unlink(FIFO_FILE);    // delete the FIFO file from filesystem
    return 0;
}


//consumer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_FILE "/tmp/myfifo"
#define BUFSIZ 512

int main() {
    int fd;
    char buffer[BUFSIZ];
    ssize_t num_bytes;

    fd = open(FIFO_FILE, O_RDONLY);        // open the SAME file, read only
    if (fd == -1) { perror("open"); exit(1); }

    while (1) {
        num_bytes = read(fd, buffer, BUFSIZ);   // read what producer wrote
        if (num_bytes <= 0) break;
        printf("Consumer: Received message: %s", buffer);
        if (strncmp(buffer, "exit", 4) == 0) break;
    }

    close(fd);
    return 0;
}
