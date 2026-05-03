#include <stdio.h>
#include <pthread.h>

// shared results array — safe here because each thread writes to its own index
long results[9];

typedef struct { int index; int value; int task; } Task;
// task: 0=square, 1=cube, 2=factorial

long factorial(int n) { return n <= 1 ? 1 : n * factorial(n - 1); }

void *process(void *arg) {
    Task *t = (Task *)arg;
    if      (t->task == 0) results[t->index] = (long)t->value * t->value;
    else if (t->task == 1) results[t->index] = (long)t->value * t->value * t->value;
    else                   results[t->index] = factorial(t->value);
    return NULL;
}

int main() {
    // 9 tasks: square/cube/factorial for values 3, 4, 5
    Task tasks[9] = {
        {0, 3, 0}, {1, 3, 1}, {2, 3, 2},
        {3, 4, 0}, {4, 4, 1}, {5, 4, 2},
        {6, 5, 0}, {7, 5, 1}, {8, 5, 2}
    };

    pthread_t threads[9];
    for (int i = 0; i < 9; i++)
        pthread_create(&threads[i], NULL, process, &tasks[i]);
    for (int i = 0; i < 9; i++)
        pthread_join(threads[i], NULL);

    const char *names[] = {"Square", "Cube", "Factorial"};
    for (int v = 0; v < 3; v++) {
        int val = tasks[v * 3].value;
        printf("n = %d → square=%-6ld cube=%-6ld factorial=%ld\n",
               val, results[v*3], results[v*3+1], results[v*3+2]);
    }
    return 0;
}
