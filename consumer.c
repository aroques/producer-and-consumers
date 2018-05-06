#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "global_constants.h"
#include "helpers.h"
#include "shared_memory.h"

void add_signal_handler();
void handle_sigint(int s);
void open_logfile(int i);

struct SharedMemory* shmem;
FILE* log_fp;

int main (int argc, char *argv[]) {
    int i, j, k, sleep_time;
    char buff[100];
    srand(time(0) ^ getpid());

    i = atoi(argv[1]);
    open_logfile(i);

    const int NUM_PROC = atoi(argv[2]);

    sprintf(buff, "Consumer %2d: %s Started\n", i, get_timestamp());
    print_and_write(buff, log_fp);

    add_signal_handler();

    char** ids = split_string(argv[ID_STR_IDX], ",");

    struct SharedMemoryIDs* shmem_ids = get_shared_memory_ids(ids);
    shmem = attach_consumer_shared_memory(shmem_ids);

    // Unpack shared memory pointers
    int* buffer_flag = shmem->buffer_flag;
    //char* buffer = shmem-> buffer;
	int* turn = shmem->turn;
	int* flag = shmem->flag; /* Flag corresponding to each process in shared memory */
    int* end_program = shmem->end_program;

    do {
        if (*end_program) {
            sprintf(buff, "Consumer %2d: %s Exiting program\n", i, get_timestamp());
            print_and_write(buff, log_fp);

            handle_sigint(-1);
        }

        do {
            if (*end_program) {
                sprintf(buff, "Consumer %2d: %s Exiting program\n", i, get_timestamp());
                print_and_write(buff, log_fp);

                handle_sigint(-1);
            }

            flag[i] = want_in; // Raise my flag

            j = *turn; // Set local variable

            // wait until its my turn
            while ( j != i ) {
                j = (flag[j] != idle) ? *turn : (j + 1) % NUM_PROC;
            }

            // Declare intention to enter critical section
            flag[i] = in_cs;

            // Check that no one else is in critical section
            sprintf(buff, "Consumer %2d: %s Checked\n", i, get_timestamp());
            print_and_write(buff, log_fp);

            for (j = 0; j < NUM_PROC; j++)
                if ((j != i) && (flag[j] == in_cs))
                    break;
        } while ((j < NUM_PROC) || (*turn != i && flag[*turn] != idle));

        // Assign turn to self and enter critical section
        *turn = i;

        for (k = 0; k < NUM_BUFFERS; k++) {
            if (buffer_flag[k] == 1) {
                // Buffer is full so read it
                sprintf(buff, "Consumer %2d: %s Read \t %d \t Message\n", i, get_timestamp(), k);
                print_and_write(buff, log_fp);
                
                buffer_flag[k] = 0;
                break;
            }
        }

        // Exit section
        j = (*turn + 1) % NUM_PROC;
        while (flag[j] == idle) {
            j = (j + 1) % NUM_PROC;
        }

        // Assign turn to next waiting process; change own flag to idle
        *turn = j; flag[i] = idle;

        // Remainder section
        sleep_time = get_sleep_time();

        sprintf(buff, "Consumer %2d: %s Sleep \t %d\n", i, get_timestamp(), sleep_time);
        print_and_write(buff, log_fp);

        sleep(sleep_time);

        } while (1);
}

void add_signal_handler() {
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_handler = handle_sigterm; // Signal handler
    sigemptyset(&act.sa_mask);       // No other signals should be blocked
    act.sa_flags = 0;                // 0 so do not modify behavior
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

void handle_sigint(int sig) {
    printf("Consumer   : Caught SIGINT\n");
    detach_shared_memory(shmem);
    fclose(log_fp);
    _exit(0);
}

void open_logfile(int i) {
    char filename[50];
    sprintf(filename, "./consumer%d.log", i);
    log_fp = fopen(filename, "w");
    if (log_fp == NULL) {
        perror("failed to open logfile for writing\n");
        exit(1);
    }
}