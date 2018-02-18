#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <getopt.h>
#include <sys/shm.h>
#include <sys/stat.h>

const int PROC_LIMIT = 20;

void print_usage();
int* parse_cmd_line_args(int argc, char *argv[]);

int main (int argc, char *argv[]) {
    int n = 10;                         // Default number of consumers
    int* num_consumers = &n;            // Number of consumers to fork
    int proc_count = 0;                 // Number of concurrent children
    pid_t childpid = 0;                 // Child process ID
    int one_producer = 0;               // True if exec'd one producer

    char* program[2];
    program[1] = NULL;

    key_t key = 1;
    struct shmid_ds shmbuffer;
    char* shared_memory;
    int segment_size;

    int segment_id = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | S_IRUSR | S_IWUSR); // children inherit shmem segments

    /* Attach the shared memory segment. */
    shared_memory = (char*)shmat(segment_id, 0, 0);
    if (!shared_memory) { /* operation failed. */
        perror("shmat: ");
        exit(1);
    }
    printf("shared memory attached at address %p\n", shared_memory);

    /* Determine the segment’s size. */
    shmctl(segment_id, IPC_STAT, &shmbuffer);
    segment_size = shmbuffer.shm_segsz;
    printf("segment size: %d\n", segment_size);

    /* Write a string to the shared memory segment. */
    sprintf(shared_memory, "Hello, world.");

    num_consumers = parse_cmd_line_args(argc, argv);

    for (int i = 0; i < 4; i++) {

        if (proc_count == PROC_LIMIT) {
            // Wait for one child to finish and decrement proc_count
            wait(NULL);
            proc_count -= 1;
        }

        if ((childpid = fork()) == 0) {
            // Child so...
            if (one_producer) {
                program[0] = "./consumer";
            }
            else {
                program[0] = "./producer";
            }

            execvp(program[0], program);

            perror("Child failed to execvp the command!");
            return 1;
        }

        if (childpid == -1) {
            perror("Child failed to fork!\n");
            return 1;
        }

        one_producer = 1;
        proc_count += 1; // increment because we forked

        if (waitpid(-1, NULL, WNOHANG) > 0) {
            // A child has finished executing
            proc_count -= 1;
        }

    }

    if (childpid > 0) {
        while (wait(NULL) > 0); // wait for all children
    }

    /* Print out the string from shared memory. */
    printf("%s\n", shared_memory);
    /* Detach the shared memory segment. */
    shmdt(shared_memory);
    /* Deallocate the shared memory segment. */
    shmctl(segment_id, IPC_RMID, 0);

    free(num_consumers);

    return 0;

}

int* parse_cmd_line_args(int argc, char *argv[]) {
    int* n = malloc(sizeof(int));

    int option;
    while ((option = getopt (argc, argv, "n:h")) != -1)
    switch (option) {
        case 'h':
            print_usage();
            break;
        case 'n':
            *n = atoi(optarg);
            break;
        default:
            print_usage();
    }

    return n;

}

void print_usage() {
    fprintf(stderr, "Usage: master [-n number of consumers]\n");
    exit(0);
}
