#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void sigalrm_handler(int signum) {
    printf("Time's up!\n");
    exit(0);
}

int main() {
    signal(SIGALRM, sigalrm_handler);  // register handler

    alarm(5);  // set alarm for 5 seconds

    printf("Alarm set for 5 seconds...\n");

    while(1) {
        printf("Program running...\n");
        sleep(1);
    }

    return 0;
}
