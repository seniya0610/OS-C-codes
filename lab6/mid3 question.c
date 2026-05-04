#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define PIPE1 "/tmp/pipe_scores"    // Score Producer  → Grade Calculator
#define PIPE2 "/tmp/pipe_grades"    // Grade Calculator → Result Reporter

#define NUM_STUDENTS 6

// ─── FAST-NUCES Grading Scheme ─────────────────────────────
const char* get_grade(int score) {
    if (score >= 85) return "A";
    if (score >= 80) return "A-";
    if (score >= 75) return "B+";
    if (score >= 71) return "B";
    if (score >= 68) return "B-";
    if (score >= 64) return "C+";
    if (score >= 61) return "C";
    if (score >= 58) return "C-";
    if (score >= 54) return "D+";
    if (score >= 50) return "D";
    return "F";
}

// ─── Component 1: Score Producer ───────────────────────────
void score_producer() {
    int scores[NUM_STUDENTS] = {90, 78, 69, 53, 47, 35};

    printf("Score Producer: Sending student scores...\n");
    fflush(stdout);

    int fd = open(PIPE1, O_WRONLY);

    // send all scores through pipe1, one by one
    for (int i = 0; i < NUM_STUDENTS; i++)
        write(fd, &scores[i], sizeof(int));

    close(fd);
}

// ─── Component 2: Grade Calculator ─────────────────────────
void grade_calculator() {
    printf("Grade Calculator: Calculating grades...\n");
    fflush(stdout);

    int fd_in  = open(PIPE1, O_RDONLY);   // read scores from pipe1
    int fd_out = open(PIPE2, O_WRONLY);   // send results to pipe2

    for (int i = 0; i < NUM_STUDENTS; i++) {
        int score;
        read(fd_in, &score, sizeof(int));         // receive raw score

        const char *grade = get_grade(score);     // compute grade

        // pack result into one string: "90 A" and send through pipe2
        char result[32];
        sprintf(result, "%d %s", score, grade);
        write(fd_out, result, sizeof(result));
    }

    close(fd_in);
    close(fd_out);
}

// ─── Component 3: Result Reporter ──────────────────────────
void result_reporter() {
    printf("Result Reporter: Displaying and logging student grades...\n");
    fflush(stdout);

    int fd = open(PIPE2, O_RDONLY);   // read results from pipe2

    FILE *file = fopen("grades_report.txt", "w");
    if (!file) { perror("fopen"); exit(1); }

    for (int i = 0; i < NUM_STUDENTS; i++) {
        char result[32];
        read(fd, result, sizeof(result));   // receive "score grade" string

        // parse the string back into score and grade
        int score;
        char grade[8];
        sscanf(result, "%d %s", &score, grade);

        // print to console
        printf("Student %d Score: %d Grade: %s\n", i + 1, score, grade);

        // save to file
        fprintf(file, "Student %d Score: %d Grade: %s\n", i + 1, score, grade);
    }

    printf("System: All student grades processed and logged successfully.\n");

    fclose(file);
    close(fd);
}

// ─── Main (Supervisor) ─────────────────────────────────────
int main() {
    // create both named pipes before forking
    mkfifo(PIPE1, 0666);
    mkfifo(PIPE2, 0666);

    pid_t pid1 = fork();
    if (pid1 == 0) { score_producer();   exit(0); }

    pid_t pid2 = fork();
    if (pid2 == 0) { grade_calculator(); exit(0); }

    pid_t pid3 = fork();
    if (pid3 == 0) { result_reporter();  exit(0); }

    // wait for all 3 children to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // cleanup: remove both pipes
    unlink(PIPE1);
    unlink(PIPE2);

    return 0;
}
