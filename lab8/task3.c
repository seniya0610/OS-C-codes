#include <stdio.h>
#include <pthread.h>

#define TOTAL    8
#define PASS_MARK 40

int marks[TOTAL] = {55, 90, 33, 72, 48, 15, 88, 61};

// ── Thread functions ──────────────────────────────────────────────────────────

void *calc_average(void *arg) {
    int sum = 0;
    for (int i = 0; i < TOTAL; i++) sum += marks[i];
    // average * 100 stored as integer to avoid malloc
    return (void *)(long)(sum * 100 / TOTAL);
}

void *find_highest(void *arg) {
    int high = marks[0];
    for (int i = 1; i < TOTAL; i++)
        if (marks[i] > high) high = marks[i];
    return (void *)(long)high;
}

void *find_lowest(void *arg) {
    int low = marks[0];
    for (int i = 1; i < TOTAL; i++)
        if (marks[i] < low) low = marks[i];
    return (void *)(long)low;
}

void *count_passed(void *arg) {
    int count = 0;
    for (int i = 0; i < TOTAL; i++)
        if (marks[i] >= PASS_MARK) count++;
    return (void *)(long)count;
}

// ── Main ──────────────────────────────────────────────────────────────────────

int main() {
    pthread_t t_avg, t_high, t_low, t_pass;
    void *avg_ret, *high_ret, *low_ret, *pass_ret;

    pthread_create(&t_avg,  NULL, calc_average, NULL);
    pthread_create(&t_high, NULL, find_highest, NULL);
    pthread_create(&t_low,  NULL, find_lowest,  NULL);
    pthread_create(&t_pass, NULL, count_passed, NULL);

    pthread_join(t_avg,  &avg_ret);
    pthread_join(t_high, &high_ret);
    pthread_join(t_low,  &low_ret);
    pthread_join(t_pass, &pass_ret);

    printf("=== Student Statistics ===\n");
    printf("Average score : %.2f\n", (long)avg_ret / 100.0);
    printf("Highest score : %ld\n",  (long)high_ret);
    printf("Lowest score  : %ld\n",  (long)low_ret);
    printf("Students passed: %ld / %d\n", (long)pass_ret, TOTAL);

    return 0;
}
