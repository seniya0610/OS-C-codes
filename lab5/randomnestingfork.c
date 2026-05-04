#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    // three shared variables — each process will mess with them
    int x = 10;
    int y = 20;
    int z = 30;

    printf("=== STARTING VALUES: x=%d  y=%d  z=%d ===\n\n", x, y, z);

    // ─── FORK 1 ────────────────────────────────────────────
    pid_t p1 = fork();

    if (p1 > 0) {
        // ── PARENT ──────────────────────────────────────────
        x = x * 3;          // x = 10*3 = 30
        y = y + x;          // y = 20+30 = 50
        z = x - y + z;      // z = 30-50+30 = 10

        printf("[PARENT]       PID:%d  x=%d  y=%d  z=%d\n",
                getpid(), x, y, z);
        // parent sees: x=30  y=50  z=10

        wait(NULL);         // wait for child 1

    } else {
        // ── CHILD 1 ─────────────────────────────────────────
        // child starts with original values: x=10 y=20 z=30
        // (fork copies the values at the moment of forking)
        x = x + y + z;      // x = 10+20+30 = 60
        y = x / 3;          // y = 60/3 = 20
        z = x * y - z;      // z = 60*20-30 = 1170

        printf("[CHILD 1]      PID:%d  x=%d  y=%d  z=%d\n",
                getpid(), x, y, z);
        // child 1 sees: x=60  y=20  z=1170

        // ── FORK 2 (inside child 1) ──────────────────────────
        pid_t p2 = fork();

        if (p2 > 0) {
            // ── CHILD 1 continues as "parent" of child 2 ────
            x = x - y * 2;     // x = 60 - 20*2 = 20
            y = z / x;         // y = 1170/20 = 58
            z = y * y - x;     // z = 58*58-20 = 3344-20 = 3324

            printf("[CHILD 1 after child2 fork]  PID:%d  x=%d  y=%d  z=%d\n",
                    getpid(), x, y, z);
            // child 1 now sees: x=20  y=58  z=3324

            wait(NULL);         // wait for child 2

        } else {
            // ── CHILD 2 (grandchild) ─────────────────────────
            // starts with child 1's values at fork moment:
            // x=60  y=20  z=1170
            x = x % 7;          // x = 60%7 = 4
            y = z - x * 100;    // y = 1170 - 4*100 = 770
            z = x + y + z;      // z = 4+770+1170 = 1944

            printf("[CHILD 2 / GRANDCHILD]       PID:%d  x=%d  y=%d  z=%d\n",
                    getpid(), x, y, z);
            // grandchild sees: x=4  y=770  z=1944

            // ── FORK 3 (inside grandchild) ───────────────────
            pid_t p3 = fork();

            if (p3 > 0) {
                // ── GRANDCHILD continues as "parent" of child 3
                x = z / (y / 10);   // x = 1944 / (770/10) = 1944/77 = 25
                y = x * x + z;      // y = 25*25+1944 = 625+1944 = 2569
                z = y - z + x;      // z = 2569-1944+25 = 650

                printf("[GRANDCHILD after child3 fork] PID:%d  x=%d  y=%d  z=%d\n",
                        getpid(), x, y, z);
                // grandchild now sees: x=25  y=2569  z=650

                wait(NULL);     // wait for great-grandchild

            } else {
                // ── CHILD 3 (great-grandchild) ───────────────
                // starts with grandchild's values at fork moment:
                // x=4  y=770  z=1944
                x = (x * y) % z;    // x = (4*770)%1944 = 3080%1944 = 1136
                y = x ^ z;          // y = 1136 XOR 1944 = ?
                z = (x + y) / 2;    // z = average of x and y

                // let's trace the XOR manually:
                // 1136 in binary = 10001110000
                // 1944 in binary = 11110011000
                // XOR            = 01111101000 = 1000
                // so y = 1000... wait let me be precise:
                // 1136 = 0100 0111 0000
                // 1944 = 0111 1001 1000
                // XOR  = 0011 1110 1000 = 1000 (decimal)
                // actually: 1136 XOR 1944:
                // 1136 = 10001110000
                // 1944 = 11110011000
                // XOR  = 01111101000 = 1000

                printf("[GREAT-GRANDCHILD]           PID:%d  x=%d  y=%d  z=%d\n",
                        getpid(), x, y, z);
            }
        }
    }

    return 0;
}
