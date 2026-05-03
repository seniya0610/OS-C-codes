#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_THREADS 4
#define FILENAME "server.log"

// Each thread gets this info
typedef struct {
    long start_byte;  // where to start reading
    long end_byte;    // where to stop reading
    int line_count;   // result: how many lines found
} ThreadData;

// ──────────────────────────────────────────────
// This function runs in each thread
// ──────────────────────────────────────────────
void *count_lines(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        perror("fopen");
        data->line_count = 0;
        return NULL;
    }

    // Jump to this thread's starting position
    fseek(file, data->start_byte, SEEK_SET);

    int count = 0;
    char line[1024];

    // Read lines until we pass our end boundary
    while (ftell(file) < data->end_byte && fgets(line, sizeof(line), file)) {
        count++;
    }

    data->line_count = count;
    fclose(file);
    return NULL;
}

int main() {
    // ── Step 1: Find total file size ──────────────
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        perror("Cannot open file");
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fclose(file);

    printf("File size: %ld bytes\n", file_size);
    printf("Splitting across %d threads...\n\n", NUM_THREADS);

    // ── Step 2: Divide file into equal chunks ─────
    ThreadData thread_data[NUM_THREADS];
    long chunk = file_size / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start_byte = i * chunk;
        thread_data[i].end_byte   = (i == NUM_THREADS - 1) ? file_size : (i + 1) * chunk;
        thread_data[i].line_count = 0;
    }

    // ── Step 3: Launch all threads ────────────────
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, count_lines, &thread_data[i]);
        printf("Thread %d: reading bytes %ld → %ld\n",
               i, thread_data[i].start_byte, thread_data[i].end_byte);
    }

    // ── Step 4: Wait for all threads to finish ────
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // ── Step 5: Add up results ────────────────────
    int total = 0;
    printf("\n--- Results ---\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Thread %d counted: %d lines\n", i, thread_data[i].line_count);
        total += thread_data[i].line_count;
    }

    printf("\n✅ Total requests processed: %d\n", total);
    return 0;
}
