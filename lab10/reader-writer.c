#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int read_count = 0;

sem_t mutex;        // protects read_count
sem_t rw_mutex;     // controls resource
sem_t queue;        // fairness (turnstile)

// Reader
void* reader(void* arg) {
    int id = *(int*)arg;

    while (1) {
        sem_wait(&queue);     // wait in line
        sem_wait(&mutex);

        read_count++;
        if (read_count == 1)
            sem_wait(&rw_mutex);

        sem_post(&mutex);
        sem_post(&queue);     // allow next thread

        printf("Reader %d is reading\n", id);
        sleep(1);

        sem_wait(&mutex);
        read_count--;

        if (read_count == 0)
            sem_post(&rw_mutex);

        sem_post(&mutex);

        sleep(1);
    }
}

// Writer
void* writer(void* arg) {
    int id = *(int*)arg;

    while (1) {
        sem_wait(&queue);     // wait in line
        sem_wait(&rw_mutex);

        sem_post(&queue);     // let others line up

        printf("Writer %d is writing\n", id);
        sleep(2);

        sem_post(&rw_mutex);

        sleep(1);
    }
}

int main() {
    pthread_t r[3], w[2];
    int ids[5];

    sem_init(&mutex, 0, 1);
    sem_init(&rw_mutex, 0, 1);
    sem_init(&queue, 0, 1);

    for (int i = 0; i < 3; i++) {
        ids[i] = i + 1;
        pthread_create(&r[i], NULL, reader, &ids[i]);
    }

    for (int i = 0; i < 2; i++) {
        ids[i+3] = i + 1;
        pthread_create(&w[i], NULL, writer, &ids[i+3]);
    }

    for (int i = 0; i < 3; i++) pthread_join(r[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(w[i], NULL);

    return 0;
}
