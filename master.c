#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <getopt.h>
#include <sys/time.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include "global_constants.h"
#include "shared_memory.h"
#include "helpers.h"

void print_usage();
int parse_cmd_line_args(int argc, char *argv[]);
char* get_program(int one_producer);
void wait_for_all_children();
char* get_child_idx(int proc_count);
char* get_ids(struct SharedMemoryIDs* shmem_ids);
void set_timer();
void add_signal_handlers();
void handle_sigint(int s);
void handle_sigalrm(int s);
void handle_sigusr1(int sig);
void terminate_children();
void cleanup_before_exit();
void initialize_shmem(struct SharedMemoryIDs* shmem_ids);
char* get_num_total_processes(int num_consumers);
void kill_process(int pid);
void cleanup_and_exit();

// Globals for cleanup in signal handler
pid_t childpids[20] = { 0 };
struct SharedMemoryIDs* shmem_ids;
struct SharedMemory* shmem;
bool cleaning_up = 0;
int* end_program;
int num_consumers;

int main (int argc, char *argv[]) {
    num_consumers = 10;                // Number of consumers to fork
    int proc_count = 0;                // Number of concurrent children
    bool one_producer = 0;             // True if exec'd one producer
    int i;

    char* execv_arr[5];
    execv_arr[4] = NULL;

    shmem_ids = get_shared_memory();
    initialize_shmem(shmem_ids);

    shmem = attach_consumer_shared_memory(shmem_ids);
    end_program = shmem->end_program;

    set_timer();
    add_signal_handlers();

    num_consumers = parse_cmd_line_args(argc, argv);

    for (i = 0; i < num_consumers + 1; i++) { // (num_consumers + 1) because 1 producer

        if (proc_count == PROC_LIMIT) {
            // Wait for one child to finish and decrement proc_count
            wait(NULL);
            proc_count -= 1;
        }

        if ((childpids[i] = fork()) == 0) {
            // Child so...
            execv_arr[0] = get_program(one_producer);

            execv_arr[1] = get_child_idx(proc_count);

            execv_arr[2] = get_num_total_processes(num_consumers);

            execv_arr[ID_STR_IDX] = get_ids(shmem_ids);

            execvp(execv_arr[0], execv_arr);

            perror("Child failed to execvp the command!");
            return 1;
        }

        if (childpids[i] == -1) {
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
    
    // wait_for_all_children();
    printf("MASTER WAITING FOR end_program\n");
    while (!*end_program);

    cleanup_and_exit();

    return 0;

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
    sprintf(child_idx, "%d", (proc_count));
    return child_idx;
}

char* get_num_total_processes(int num_consumers) {
    char* n = malloc(sizeof(char)*3);
    sprintf(n, "%d", (num_consumers + 1));
    return n;
}

char* get_ids(struct SharedMemoryIDs* shmem_ids) {
    char* ids = malloc(sizeof(char)*25);
    sprintf(ids, "%d,%d,%d,%d,%d", shmem_ids->buffer_id, shmem_ids->flag_id,
            shmem_ids->turn_id, shmem_ids->buffer_flag_id, shmem_ids->end_program_id);
    return ids;
}

int parse_cmd_line_args(int argc, char* argv[]) {
    int n = 0;

    int option;
    while ((option = getopt (argc, argv, "n:h")) != -1)
    switch (option) {
        case 'h':
            print_usage();
            break;
        case 'n':
            n = atoi(optarg);
            break;
        default:
            print_usage();
    }

    if (n == 0) {
        return 10;
    }
    else {
        return n;
    }

}

void print_usage() {
    fprintf(stderr, "Usage: master [-n number of consumers]\n");
    exit(0);
}

void wait_for_all_children() {
    pid_t pid;
    printf("Master: Waiting for all children to exit\n");
    
    while ((pid = wait(NULL))) {
        if (pid < 0) {
            if (errno == ECHILD) {
                perror("wait");
                break;
            }
        }
    }
}

void add_signal_handlers() {
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_handler = handle_sigint; // Signal handler
    sigemptyset(&act.sa_mask);      // No other signals should be blocked
    act.sa_flags = 0;               // 0 so do not modify behavior
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    act.sa_handler = handle_sigalrm; // Signal handler
    sigemptyset(&act.sa_mask);       // No other signals should be blocked
    if (sigaction(SIGALRM, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    act.sa_handler = handle_sigusr1; // Signal handler
    sigemptyset(&act.sa_mask);       // No other signals should be blocked
    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

void handle_sigint(int sig) {
    printf("\nMaster: Caught SIGINT signal %d\n", sig);
    printf("\nMaster: BEFORE SETTING end_program %d\n", *end_program);
    *end_program = 1;
    printf("\nMaster: AFTER SETTING end_program %d\n", *end_program);
    if (cleaning_up == 0) {
        cleaning_up = 1;
        cleanup_and_exit();
    }
}

void handle_sigalrm(int sig) {
    printf("\nMaster: Caught SIGALRM signal %d\n", sig);
    printf("Master: %d seconds have passed\n", TIMER_DURATION);
    if (cleaning_up == 0) {
        cleaning_up = 1;
        cleanup_and_exit();
    }
}

void handle_sigusr1(int sig) {
    printf("\nMaster: Caught SIGUSR1 signal %d\n", sig);
    printf("Master: Producer has signaled that all the lines in the file have been read\n");
    if (cleaning_up == 0) {
        cleaning_up = 1;
        cleanup_and_exit();
    }
}

void cleanup_and_exit() {
    // sleep(10);
    // terminate_children();
    printf("Master: cleanup_and_exit()\n");
    wait_for_all_children();
    printf("Master: Removing shared memory\n");
    cleanup_shared_memory(shmem_ids, shmem);
    free(shmem_ids);
    free(shmem);
    exit(0);
}

void terminate_children() {
    printf("Master: Sending SIGTERM to all children\n");
    int i;
    for (i = 0; i < (num_consumers + 1); i++) {
        if (childpids[i] == 0) {
            continue;
        }
        kill_process(i);
    }
}

void kill_process(int pid) {
    printf("SENDING SIGTERM TO CHILD PID %d\n", childpids[pid]);
    if (kill(childpids[pid], SIGTERM) < 0) {
        if (errno != ESRCH) {
            // Child process exists and kill failed
            perror("kill");
        }
    }
}

void set_timer() {
  struct itimerval value;
  value.it_interval.tv_sec = TIMER_DURATION;
  value.it_interval.tv_usec = 0;
  value.it_value = value.it_interval;
  if (setitimer(ITIMER_REAL, &value, NULL) == -1) {
      perror("Failed to set up interval timer");
      exit(1);
  }
}

void initialize_shmem(struct SharedMemoryIDs* shmem_ids) {
    struct SharedMemory* shmem = attach_shared_memory(shmem_ids);
    initialize_shared_memory(shmem);
    detach_shared_memory(shmem);
    free(shmem);
}

