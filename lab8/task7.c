#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>

#define NUM_FILES 3

const char *filenames[NUM_FILES] = {"file1.txt", "file2.txt", "file3.txt"};

void *count_words(void *arg) {
    const char *fname = (const char *)arg;
    FILE *f = fopen(fname, "r");
    if (!f) { printf("Cannot open %s\n", fname); return (void *)0; }

    int count = 0, in_word = 0;
    char c;
    while ((c = fgetc(f)) != EOF) {
        if (isspace(c)) in_word = 0;
        else if (!in_word) { in_word = 1; count++; }
    }
    fclose(f);
    return (void *)(long)count;
}

int main() {
    // Create test files
    FILE *f;
    f = fopen("file1.txt", "w"); fprintf(f, "hello world this is file one"); fclose(f);
    f = fopen("file2.txt", "w"); fprintf(f, "the quick brown fox jumps"); fclose(f);
    f = fopen("file3.txt", "w"); fprintf(f, "multithreading in C is powerful and fast"); fclose(f);

    pthread_t threads[NUM_FILES];
    for (int i = 0; i < NUM_FILES; i++)
        pthread_create(&threads[i], NULL, count_words, (void *)filenames[i]);

    int total = 0;
    void *result;
    for (int i = 0; i < NUM_FILES; i++) {
        pthread_join(threads[i], &result);
        int count = (int)(long)result;
        printf("%-12s → %d words\n", filenames[i], count);
        total += count;
    }
    printf("Total words: %d\n", total);
    return 0;
}
