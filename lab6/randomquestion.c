#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define SHM_NAME "supervisor_shm"
#define SIZE sizeof(SharedData)

typedef struct {
    int student_id;
    int attendance;
    char att_status[20];

    char student_name[50];
    int assigned_id;

    char task_name[50];
    int task_done;
    char task_status[20];

    int start_worker1;
    int start_worker2;
    int start_worker3;

    int done_worker1;
    int done_worker2;
    int done_worker3;
} SharedData;

// ─── Worker 1: Update Attendance ───────────────────────────
void worker_attendance(SharedData *data) {
    printf("[Worker 1] Waiting for task...\n");
    while (!data->start_worker1);   // spin until manager says go

    printf("[Worker 1] Updating attendance for ID %d\n", data->student_id);
    sleep(1);   // simulate doing work

    if (data->attendance == 1)
        sprintf(data->att_status, "Present");
    else
        sprintf(data->att_status, "Absent");

    printf("[Worker 1] Done. Status: %s\n", data->att_status);
    data->done_worker1 = 1;   // tell manager i'm done
}

// ─── Worker 2: Give ID ─────────────────────────────────────
void worker_give_id(SharedData *data) {
    printf("[Worker 2] Waiting for task...\n");
    while (!data->start_worker2);

    printf("[Worker 2] Assigning ID to student: %s\n", data->student_name);
    sleep(1);

    // simple ID generation: sum of ASCII values of name
    int id = 0;
    for (int i = 0; data->student_name[i] != '\0'; i++)
        id += data->student_name[i];
    data->assigned_id = id % 9000 + 1000;   // gives a 4-digit number

    printf("[Worker 2] Done. Assigned ID: %d\n", data->assigned_id);
    data->done_worker2 = 1;
}

// ─── Worker 3: Update Task Status ──────────────────────────
void worker_task_status(SharedData *data) {
    printf("[Worker 3] Waiting for task...\n");
    while (!data->start_worker3);

    printf("[Worker 3] Checking task: %s\n", data->task_name);
    sleep(1);

    if (data->task_done == 1)
        sprintf(data->task_status, "Completed");
    else
        sprintf(data->task_status, "Pending");

    printf("[Worker 3] Done. Task status: %s\n", data->task_status);
    data->done_worker3 = 1;
}

// ─── Main (Supervisor/Manager) ─────────────────────────────
int main() {
    // --- Setup shared memory ---
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    SharedData *data = (SharedData *)mmap(0, SIZE,
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // zero everything out
    memset(data, 0, SIZE);

    // --- Manager fills in the tasks ---
    printf("[Manager] Setting up tasks...\n\n");

    data->student_id  = 42;
    data->attendance  = 1;

    strcpy(data->student_name, "Seniya");

    strcpy(data->task_name, "Submit OS Lab Report");
    data->task_done = 0;

    // --- Fork 3 workers ---
    pid_t pid1 = fork();
    if (pid1 == 0) { worker_attendance(data); exit(0); }

    pid_t pid2 = fork();
    if (pid2 == 0) { worker_give_id(data); exit(0); }

    pid_t pid3 = fork();
    if (pid3 == 0) { worker_task_status(data); exit(0); }

    // --- Manager signals all workers to start ---
    sleep(1);   // give workers a moment to start and reach their wait loop
    printf("[Manager] Signaling all workers to begin...\n\n");
    data->start_worker1 = 1;
    data->start_worker2 = 1;
    data->start_worker3 = 1;

    // --- Manager waits for all workers to finish ---
    while (!data->done_worker1 || !data->done_worker2 || !data->done_worker3);

    // --- Manager reads and reports results ---
    printf("\n[Manager] All workers done. Collecting results...\n");
    printf("─────────────────────────────────────────\n");
    printf("  Attendance  → Student ID %d : %s\n", data->student_id, data->att_status);
    printf("  Student ID  → %s : %d\n", data->student_name, data->assigned_id);
    printf("  Task Status → \"%s\" : %s\n", data->task_name, data->task_status);
    printf("─────────────────────────────────────────\n");

    // --- Cleanup ---
    wait(NULL); wait(NULL); wait(NULL);
    shm_unlink(SHM_NAME);
    return 0;
}
