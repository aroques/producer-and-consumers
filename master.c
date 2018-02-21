#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <getopt.h>
#include <sys/time.h>

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
void sig_handler(int s);
void kill_all_children();
void cleanup_before_exit();
void initialize_shmem(struct SharedMemoryIDs* shmem_ids);
char* get_num_total_processes(int num_consumers);
void eof_reached(int s);

// Globals for cleanup in signal handler
pid_t childpids[20] = { 0 };
struct SharedMemoryIDs* shmem_ids;

int main (int argc, char *argv[]) {
    int num_consumers = 10;            // Number of consumers to fork
    int proc_count = 0;                // Number of concurrent children
    int one_producer = 0;              // True if exec'd one producer

    char* execv_arr[5];
    execv_arr[4] = NULL;

    shmem_ids = get_shared_memory();

    set_timer();
    add_signal_handlers();

    num_consumers = parse_cmd_line_args(argc, argv);

    initialize_shmem(shmem_ids);

    for (int i = 0; i < num_consumers + 1; i++) { // (num_consumers + 1) because 1 producer

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

    cleanup_before_exit();

    return 0;

}

void cleanup_before_exit() {
    wait_for_all_children();

    deallocate_shared_memory(shmem_ids);

    free(shmem_ids);
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
    char* ids = malloc(sizeof(char)*10);
    sprintf(ids, "%d,%d,%d,%d", shmem_ids->buffer_id, shmem_ids->flag_id,
            shmem_ids->turn_id, shmem_ids->buffer_flag_id);
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
    pid_t childpid;
    int status;
    while  ( (childpid = wait(&status) ) > 0) {
        printf("child exited. pid: %d status %d\n", childpid, status);
    }
}

void add_signal_handlers(void) {
  struct sigaction act;
  act.sa_handler = sig_handler;
  act.sa_flags = 0;
  if ( ( sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGALRM, &act, NULL)  == -1) ||
          (sigaction(SIGINT, &act, NULL)  == -1) ) {
      perror("Failed to set up interrupt");
      exit(1);
  }

  act.sa_handler = eof_reached;
  act.sa_flags = 0;
  if ( ( sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGUSR1, &act, NULL)  == -1) ) {
      perror("Failed to set up interrupt");
      exit(1);
  }

}

void eof_reached(int s) {
  kill_all_children(SIGUSR1);
  printf("Sending SIGUSR1 from producer to parent\n");
  cleanup_before_exit();
  exit(1);
}

void sig_handler(int s) {
  kill_all_children(SIGINT);
  cleanup_before_exit();
  exit(1);
}

void kill_all_children(const int signal) {
    int length = sizeof(childpids)/sizeof(childpids[0]);
    for (int i = 0; i < length; i++) {
        if (childpids[i] > 0) {
            kill(childpids[i], signal);
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

