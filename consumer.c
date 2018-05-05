#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "global_constants.h"
#include "helpers.h"
#include "shared_memory.h"

void add_signal_handler();
void handle_sigterm(int s);
void open_logfile();

struct SharedMemory* shmem;
FILE* log_fp;

int main (int argc, char *argv[]) {
    int j, sleep_time;
    char buff[100];
    srand(time(0) ^ getpid());

    int i = atoi(argv[1]);

    const int NUM_PROC = atoi(argv[2]);

    sprintf(buff, "Consumer %2d: %s Started\n", i, get_timestamp());
    print_and_write(buff, log_fp);

    add_signal_handler();

    char** ids = split_string(argv[ID_STR_IDX], ",");

    struct SharedMemoryIDs* shmem_ids = get_shared_memory_ids(ids);
    shmem = attach_consumer_shared_memory(shmem_ids);

    // Unpack shared memory pointers
    int* buffer_flag = shmem->buffer_flag;
    char* buffer = shmem-> buffer;
	int* turn = shmem->turn;
	int* flag = shmem->flag; /* Flag corresponding to each process in shared memory */

    do {

        do {
            flag[i] = want_in; // Raise my flag

            j = *turn; // Set local variable

            // wait until its my turn
            while ( j != i ) {
                j = (flag[j] != idle) ? *turn : (j + 1) % NUM_PROC;
            }

            // Declare intention to enter critical section
            flag[i] = in_cs;

            // Check that no one else is in critical section
            sprintf(buff, "Consumer: %s Checked\n", get_timestamp());
            print_and_write(buff, log_fp);

            for (j = 0; j < NUM_PROC; j++)
                if ((j != i) && (flag[j] == in_cs))
                    break;
        } while ((j < NUM_PROC) || (*turn != i && flag[*turn] != idle));

        // Assign turn to self and enter critical section
        *turn = i;

        for (i = 0; i < NUM_BUFFERS; i++) {
            if (buffer_flag[i] == 1) {
                // Buffer is full so read it
                sprintf(buff, "Consumer: %s Read \t %d \t Message\n", get_timestamp(), i);
                print_and_write(buff, log_fp);
                
                buffer_flag[i] = 0;
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
        sleep_time = (rand() % 5) + 1;

        sprintf(buff, "Consumer: %s Sleep \t %d\n", get_timestamp(), sleep_time);
        print_and_write(buff, log_fp);

        sleep(sleep_time);

        } while (1);
}

void add_signal_handlers() {
    struct sigaction act;
    act.sa_handler = handle_sigterm; // Signal handler
    sigemptyset(&act.sa_mask);      // No other signals should be blocked
    act.sa_flags = 0;               // 0 so do not modify behavior
    if (sigaction(SIGTERM, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

void handle_sigterm(int sig) {
    detach_shared_memory(shmem);
    _exit(0);
}

void open_logfile() {
    char filename[50];
    sprintf(filename, "./consumer%d.log", i);
    log_fp = fopen(filename, "w");
    if (log_fp == NULL) {
        perror("failed to open %s for writing\n", filename);
        exit(1);
    }
}