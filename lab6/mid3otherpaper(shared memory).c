#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define SHM_NAME "/attendance_shm"
#define NUM_STUDENTS 5

// ─── Shared Memory Structure ────────────────────────────────
typedef struct {
    // Producer writes these
    int attendance[NUM_STUDENTS];   // 1 = Present, 0 = Absent

    // Analyzer writes these
    int total_present;
    int total_absent;

    // Sync flags (sequential: producer → analyzer → reporter)
    int producer_done;
    int analyzer_done;
} SharedData;

// ─── Component 1: Attendance Producer ──────────────────────
void attendance_producer(SharedData *data) {
    printf("Attendance Producer: Sending attendance records...\n");
    fflush(stdout);

    // raw attendance: 1=Present, 0=Absent
    int records[NUM_STUDENTS] = {1, 0, 1, 1, 0};

    for (int i = 0; i < NUM_STUDENTS; i++)
        data->attendance[i] = records[i];

    data->producer_done = 1;   // signal analyzer to start
}

// ─── Component 2: Attendance Analyzer ──────────────────────
void attendance_analyzer(SharedData *data) {
    // wait for producer to finish
    while (!data->producer_done);

    printf("Attendance Analyzer: Calculating attendance summary...\n");
    fflush(stdout);

    int present = 0, absent = 0;

    for (int i = 0; i < NUM_STUDENTS; i++) {
        if (data->attendance[i] == 1)
            present++;
        else
            absent++;
    }

    data->total_present = present;
    data->total_absent  = absent;

    data->analyzer_done = 1;   // signal reporter to start
}

// ─── Component 3: Attendance Reporter ──────────────────────
void attendance_reporter(SharedData *data) {
    // wait for analyzer to finish
    while (!data->analyzer_done);

    printf("Attendance Reporter: Displaying and logging attendance summary...\n");
    fflush(stdout);

    // print to console
    printf("Total Students Present: %d\n", data->total_present);
    printf("Total Students Absent:  %d\n", data->total_absent);
    printf("System: Attendance processed and logged successfully.\n");

    // save to file
    FILE *file = fopen("attendance_report.txt", "w");
    if (!file) { perror("fopen"); exit(1); }

    fprintf(file, "Total Students Present: %d\n", data->total_present);
    fprintf(file, "Total Students Absent:  %d\n", data->total_absent);

    fclose(file);
}

// ─── Main (Supervisor) ─────────────────────────────────────
int main() {
    // create shared memory
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(SharedData));
    SharedData *data = (SharedData *)mmap(0, sizeof(SharedData),
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // zero everything out
    memset(data, 0, sizeof(SharedData));

    // fork 3 children
    pid_t pid1 = fork();
    if (pid1 == 0) { attendance_producer(data);  exit(0); }

    pid_t pid2 = fork();
    if (pid2 == 0) { attendance_analyzer(data);  exit(0); }

    pid_t pid3 = fork();
    if (pid3 == 0) { attendance_reporter(data);  exit(0); }

    // wait for all 3 to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // cleanup
    shm_unlink(SHM_NAME);

    return 0;
}
