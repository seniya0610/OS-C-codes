#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 4

// ── Shared read-only data ─────────────────────────────────────────────────────
const char *document;
const char *keyword;
int doc_len, kw_len;

// ── Each thread's assigned segment ───────────────────────────────────────────
typedef struct {
    int start;   // start index in document
    int end;     // end index (exclusive)
} Segment;

Segment segments[NUM_THREADS];

// ── Thread function: count keyword hits in [start, end) ──────────────────────
void *search_segment(void *arg) {
    Segment *seg = (Segment *)arg;
    int count = 0;

    // Search up to end, but read kw_len chars ahead so we catch boundary spans
    int search_limit = seg->end + kw_len - 1;
    if (search_limit > doc_len) search_limit = doc_len;

    for (int i = seg->start; i < search_limit - kw_len + 1; i++) {
        if (strncmp(&document[i], keyword, kw_len) == 0)
            count++;
    }

    return (void *)(long)count;
}

int main() {
    // ── Setup ─────────────────────────────────────────────────────────────────
    document = "the cat sat on the mat and the cat wore a hat and the cat came back";
    keyword  = "cat";
    doc_len  = strlen(document);
    kw_len   = strlen(keyword);

    printf("Document : \"%s\"\n", document);
    printf("Keyword  : \"%s\"\n", keyword);
    printf("Threads  : %d\n\n", NUM_THREADS);

    // ── Divide document into segments ─────────────────────────────────────────
    int chunk = doc_len / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        segments[i].start = i * chunk;
        segments[i].end   = (i == NUM_THREADS - 1) ? doc_len : (i + 1) * chunk;
    }

    // ── Launch threads ────────────────────────────────────────────────────────
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, search_segment, &segments[i]);

    // ── Collect results ───────────────────────────────────────────────────────
    int total = 0;
    void *result;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], &result);
        int count = (int)(long)result;
        printf("Thread %d [%d..%d]: found %d\n", i, segments[i].start, segments[i].end - 1, count);
        total += count;
    }

    printf("\nTotal occurrences of \"%s\": %d\n", keyword, total);
    return 0;
}
