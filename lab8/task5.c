#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 4

int numbers[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
                 22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40};
int total = sizeof(numbers) / sizeof(numbers[0]);

typedef struct {
    int start;
    int end;
} Segment;

Segment segments[NUM_THREADS];

// ── Is prime? ─────────────────────────────────────────────────────────────────
int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0) return 0;
    return 1;
}

// ── Thread function ───────────────────────────────────────────────────────────
void *count_primes(void *arg) {
    Segment *seg = (Segment *)arg;
    int count = 0;
    for (int i = seg->start; i < seg->end; i++)
        if (is_prime(numbers[i])) count++;
    return (void *)(long)count;
}

int main() {
    int chunk = total / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        segments[i].start = i * chunk;
        segments[i].end   = (i == NUM_THREADS - 1) ? total : (i + 1) * chunk;
    }

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, count_primes, &segments[i]);

    int total_primes = 0;
    void *result;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], &result);
        int count = (int)(long)result;
        printf("Thread %d [idx %d..%d]: found %d primes\n",
               i, segments[i].start, segments[i].end - 1, count);
        total_primes += count;
    }

    printf("\nTotal prime numbers found: %d\n", total_primes);
    return 0;
}
