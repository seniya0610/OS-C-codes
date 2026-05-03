#include <stdio.h>
#include <pthread.h>

int balance = 1000;   // shared — no protection intentional

void *deposit(void *arg)    { balance += 500; return NULL; }
void *withdraw(void *arg)   { balance -= 200; return NULL; }

int main() {
    pthread_t t[6];
    // 3 deposits, 3 withdrawals — expected: 1000 + 1500 - 600 = 1900
    for (int i = 0; i < 3; i++) pthread_create(&t[i],   NULL, deposit,  NULL);
    for (int i = 3; i < 6; i++) pthread_create(&t[i],   NULL, withdraw, NULL);
    for (int i = 0; i < 6; i++) pthread_join(t[i], NULL);

    printf("Expected balance : 1900\n");
    printf("Actual balance   : %d\n", balance);
    printf("%s\n", balance != 1900 ? "RACE CONDITION OCCURRED!" : "Got lucky, run again.");
    return 0;
}
