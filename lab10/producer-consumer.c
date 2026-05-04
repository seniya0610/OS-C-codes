#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty, full;
pthread_mutex_t mutex;

// Producer
void* producer(void* arg) {
    int item;
    while (1) {
        item = rand() % 100;

        sem_wait(&empty);              // wait if buffer full
        pthread_mutex_lock(&mutex);    // enter critical section

        buffer[in] = item;
        printf("Produced: %d\n", item);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);  // exit critical section
        sem_post(&full);               // increment full count

        sleep(1);
    }
}

// Consumer
void* consumer(void* arg) {
    int item;
    while (1) {
        sem_wait(&full);               // wait if buffer empty
        pthread_mutex_lock(&mutex);    // enter critical section

        item = buffer[out];
        printf("Consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);  // exit critical section
        sem_post(&empty);              // increment empty count

        sleep(1);
    }
}

int main() {
    pthread_t p, c;

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    return 0;
}
