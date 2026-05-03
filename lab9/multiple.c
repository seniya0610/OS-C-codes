#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Use volatile sig_atomic_t for signal-safe flags
volatile sig_atomic_t paused = 0;

// 1. Pause Handler
void handle_pause(int signum) {
    paused = 1;
    // Note: write() is used for async-signal safety
    write(STDOUT_FILENO, "Signal: Pausing...\n", 19);
}

// 2. Resume Handler
void handle_resume(int signum) {
    paused = 0;
    write(STDOUT_FILENO, "Signal: Resuming...\n", 20);
}

// 3. Print Numbers Handler
void handle_print_numbers(int signum) {
    write(STDOUT_FILENO, "Signal: Printing sequence...\n", 29);
    for (int i = 1; i <= 5; i++) {
        // In a real handler, you'd avoid printf, but for logic:
        printf("  Number: %d\n", i);
    }
}

// 4. Kill/Exit Handler
void handle_exit(int signum) {
    write(STDOUT_FILENO, "Signal: Terminating program safely.\n", 36);
    exit(0);
}

int main() {
    printf("Control Center Running. PID = %d\n", getpid());
    printf("Commands:\n");
    printf("  kill -USR1 %d   (Pause)\n", getpid());
    printf("  kill -USR2 %d   (Resume)\n", getpid());
    printf("  kill -34 %d     (Print Numbers - SIGRTMIN)\n", getpid());
    printf("  kill -35 %d     (Kill - SIGRTMIN+1)\n\n", getpid());

    // Registering SIGUSR1 and SIGUSR2
    signal(SIGUSR1, handle_pause);
    signal(SIGUSR2, handle_resume);

    // Registering Real-Time Signals for the extra activities
    // SIGRTMIN is usually signal 34
    signal(SIGRTMIN, handle_print_numbers); 
    signal(SIGRTMIN + 1, handle_exit);

    while(1) {
        if (!paused) {
            printf("Processing data...\n");
            sleep(30);
        }
    }

    return 0;
}
