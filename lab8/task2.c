#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define TOTAL_ORDERS 12   // divisible by 4 for simplicity

// ── Data: array of order prices ──────────────────────────────────────────────
double orders[TOTAL_ORDERS] = {
    120.50, 340.00, 89.99,  500.00,
    210.75, 60.00,  999.99, 45.50,
    300.00, 150.00, 75.25,  430.00
};

// ── Each thread gets this "job card" ─────────────────────────────────────────
typedef struct {
    int    start;     // first index this thread processes
    int    end;       // one past the last index (exclusive)
    double subtotal;  // result written back here
} ThreadData;

// ── Thread function ───────────────────────────────────────────────────────────
void *compute_subtotal(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    data->subtotal = 0.0;
    for (int i = data->start; i < data->end; i++) {
        data->subtotal += orders[i];
    }

    printf("  Thread [%d-%d]: subtotal = $%.2f\n",
           data->start, data->end - 1, data->subtotal);

    return NULL;
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    pthread_t  threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int chunk = TOTAL_ORDERS / NUM_THREADS;  // orders per thread

    printf("Dividing %d orders across %d threads...\n\n", TOTAL_ORDERS, NUM_THREADS);

    // ── Step 1: Create threads, hand each a slice of the array ───────────────
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start    = i * chunk;
        thread_data[i].end      = (i == NUM_THREADS - 1)
                                  ? TOTAL_ORDERS          // last thread gets remainder
                                  : (i + 1) * chunk;
        thread_data[i].subtotal = 0.0;

        pthread_create(&threads[i], NULL, compute_subtotal, &thread_data[i]);
    }

    // ── Step 2: Wait for all threads to finish ────────────────────────────────
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // ── Step 3: Add up all subtotals ──────────────────────────────────────────
    double total_revenue = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        total_revenue += thread_data[i].subtotal;
    }

    printf("\n✅ Total revenue for the day: $%.2f\n", total_revenue);
    return 0;
}
