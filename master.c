#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <getopt.h>
#include <sys/shm.h>

#include "functions.h"

const int PROC_LIMIT = 20;
const int BUFFER_SIZE = 50;

void print_usage();
int* parse_cmd_line_args(int argc, char *argv[]);
char* get_program(int one_producer);
void wait_for_all_children(int childpid);
char* get_child_idx(int proc_count);
void cleanup_shmem(char* shared_memory, int* segment_id);

int main (int argc, char *argv[]) {
    int n = 10;                         // Default number of consumers
    int* num_consumers = &n;            // Number of consumers to fork
    int proc_count = 0;                 // Number of concurrent children
    pid_t childpid = 0;                 // Child process ID
    int one_producer = 0;               // True if exec'd one producer

    char* execv_arr[4];
    execv_arr[3] = NULL;

    struct shmid_ds shmbuffer;

    int segment_id = get_shared_memory();

    char* shared_memory = attach_shared_memory(segment_id);
    /* Determine the segment’s size. */
    shmctl(segment_id, IPC_STAT, &shmbuffer);
    int segment_size = shmbuffer.shm_segsz;
    printf("segment size: %d\n", segment_size);

    /* Write a string to the shared memory segment. */
    sprintf(shared_memory, "hello world!\n");

    execv_arr[2] = shared_memory;

    num_consumers = parse_cmd_line_args(argc, argv);

    for (int i = 0; i < 4; i++) {

        if (proc_count == PROC_LIMIT) {
            // Wait for one child to finish and decrement proc_count
            wait(NULL);
            proc_count -= 1;
        }

        if ((childpid = fork()) == 0) {
            // Child so...
            execv_arr[0] = get_program(one_producer);

            execv_arr[1] = get_child_idx(proc_count);

            execvp(execv_arr[0], execv_arr);

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

    wait_for_all_children(childpid);

    /* Print out the string from shared memory. */
    printf("%s\n", shared_memory);

    cleanup_shmem(shared_memory, &segment_id);

    free(num_consumers);

    return 0;

}

void cleanup_shmem(char* shared_memory, int* segment_id) {
    /* Detach the shared memory segment. */
    if (shmdt(shared_memory) == -1 ) {
        perror("shmdt");
        exit(1);
    }
    /* Deallocate the shared memory segment. */
    if (shmctl(*segment_id, IPC_RMID, 0) == 1) {
        perror("shmctl");
        exit(1);
    }
}

char* get_program(int one_producer) {
    if (one_producer) {
        return "./consumer";
    }
    else {
        return "./producer";
    }
}

char* get_child_idx(int proc_count) {
    char* child_idx = malloc(sizeof(char)*3);
    sprintf(child_idx, "%d", (proc_count - 1));
    return child_idx;
}

int* parse_cmd_line_args(int argc, char* argv[]) {
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

void wait_for_all_children(int childpid) {
    if (childpid > 0) {
        while (wait(NULL) > 0);
    }
}
