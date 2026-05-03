#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 4
#define TOTAL_REVIEWS 20

float reviews[TOTAL_REVIEWS] = {
    4.5, 3.0, 5.0, 2.5, 4.0,
    3.5, 4.0, 1.0, 5.0, 3.0,
    2.0, 4.5, 3.5, 5.0, 4.0,
    1.5, 3.0, 4.5, 2.0, 5.0
};

typedef struct { int start, end; } Segment;
Segment segments[NUM_THREADS];

// returns average * 100 as integer (same trick as Q4)
void *calc_avg(void *arg) {
    Segment *seg = (Segment *)arg;
    float sum = 0;
    for (int i = seg->start; i < seg->end; i++)
        sum += reviews[i];
    int avg100 = (int)((sum / (seg->end - seg->start)) * 100);
    return (void *)(long)avg100;
}

int main() {
    int chunk = TOTAL_REVIEWS / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        segments[i].start = i * chunk;
        segments[i].end   = (i == NUM_THREADS - 1) ? TOTAL_REVIEWS : (i + 1) * chunk;
    }

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, calc_avg, &segments[i]);

    float overall_sum = 0;
    void *result;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], &result);
        float avg = (int)(long)result / 100.0f;
        printf("Thread %d [reviews %2d..%2d]: avg = %.2f\n",
               i, segments[i].start, segments[i].end - 1, avg);
        overall_sum += avg;
    }
    printf("\nOverall average rating: %.2f\n", overall_sum / NUM_THREADS);
    return 0;
}
