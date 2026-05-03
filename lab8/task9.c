#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3

// ── Shared results array — written by threads, read by main ──────────────────
long results[NUM_THREADS];   // results[0]=square, results[1]=cube, results[2]=factorial

int n = 5;  // the number to process

// ── Thread functions — each writes to its own slot, no overlap ────────────────
void *calc_square(void *arg) {
    results[0] = (long)n * n;
    return NULL;
}

void *calc_cube(void *arg) {
    results[1] = (long)n * n * n;
    return NULL;
}

void *calc_factorial(void *arg) {
    long fact = 1;
    for (int i = 2; i <= n; i++) fact *= i;
    results[2] = fact;
    return NULL;
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    pthread_t t[NUM_THREADS];

    pthread_create(&t[0], NULL, calc_square,    NULL);
    pthread_create(&t[1], NULL, calc_cube,      NULL);
    pthread_create(&t[2], NULL, calc_factorial, NULL);

    // wait for all threads to finish writing to results[]
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(t[i], NULL);

    // safe to read results[] only after all joins
    printf("n = %d\n", n);
    printf("Square    : %ld\n", results[0]);
    printf("Cube      : %ld\n", results[1]);
    printf("Factorial : %ld\n", results[2]);

    return 0;
}
