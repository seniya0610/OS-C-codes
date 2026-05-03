#include <stdio.h>
#include <pthread.h>

#define NUM_REGIONS 4

int region_votes[NUM_REGIONS][6] = {
    {1, 2, 1, 1, 2, 1},
    {2, 2, 1, 2, 1, 2},
    {1, 1, 1, 2, 2, 1},
    {2, 1, 2, 2, 1, 2}
};
int region_size = 6;

typedef struct { int c1, c2; } Counts;

// ── One arg struct per thread: its region index + pointer to its result slot ──
typedef struct {
    int     region;
    Counts *result;   // points into counts[] in main
} ThreadArg;

void *count_votes(void *arg) {
    ThreadArg *t = (ThreadArg *)arg;
    t->result->c1 = 0;
    t->result->c2 = 0;

    for (int i = 0; i < region_size; i++) {
        if (region_votes[t->region][i] == 1) t->result->c1++;
        else                                 t->result->c2++;
    }

    printf("Region %d: c1 = %d, c2 = %d\n", t->region, t->result->c1, t->result->c2);
    return NULL;
}

int main() {
    Counts    counts[NUM_REGIONS];      // all results live here — allocated once
    ThreadArg args[NUM_REGIONS];        // one arg per thread
    pthread_t threads[NUM_REGIONS];

    for (int i = 0; i < NUM_REGIONS; i++) {
        args[i].region = i;
        args[i].result = &counts[i];   // each thread gets a pointer to its own slot
        pthread_create(&threads[i], NULL, count_votes, &args[i]);
    }

    for (int i = 0; i < NUM_REGIONS; i++)
        pthread_join(threads[i], NULL);

    int total_c1 = 0, total_c2 = 0;
    for (int i = 0; i < NUM_REGIONS; i++) {
        total_c1 += counts[i].c1;
        total_c2 += counts[i].c2;
    }

    printf("\nFinal count:\n");
    printf("Candidate 1: %d votes\n", total_c1);
    printf("Candidate 2: %d votes\n", total_c2);
    printf("Winner: Candidate %d\n", total_c1 > total_c2 ? 1 : 2);
    return 0;
}
