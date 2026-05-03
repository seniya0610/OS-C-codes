#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// child's signal handler
void sigterm_handler(int signum) {
    printf("Child terminated by parent\n");
    exit(0);  // child exits cleanly
}

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // CHILD PROCESS
        signal(SIGTERM, sigterm_handler);  // register handler

        while(1) {
            printf("Child process running...\n");
            sleep(1);
        }
    }
    else {
        // PARENT PROCESS
        printf("Parent waiting 5 seconds...\n");
        sleep(5);                    // wait 5 seconds
        kill(pid, SIGTERM);          // send SIGTERM to child
        wait(NULL);                  // wait for child to finish
        printf("Parent done\n");
    }

    return 0;
}
