#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "global_constants.h"
#include "helpers.h"
#include "shared_memory.h"

void add_signal_handler();
void sigint_handler(int s);
void eof_handler(int s);

struct SharedMemory* shmem;
FILE* log_fp;

int main (int argc, char *argv[]) {
    int j, sleep_time, buffer_idx = 0;
    srand(time(0));
    for (int i = 0; i < 5; i++) {
        //printf("argv[%d] = %s\n", i, argv[i]);
    }

    //printf("\n");
    int i = atoi(argv[1]);
    //printf("i = %d\n", i);
    const int NUM_PROC = atoi(argv[2]);

//    char filename[50];
//    sprintf(filename, "./consumer%d.log", i);
//    log_fp = fopen(filename, "w");
//    if (log_fp == NULL) {
//        perror("failed to open data.txt for reading\n");
//        exit(1);
//    }
//    fprintf(log_fp, "%s Started\n", get_timestamp());

    printf("Consumer %2d: %s Started\n", i, get_timestamp());

    add_signal_handler();

    char** ids = split_string(argv[ID_STR_IDX], ",");

    struct SharedMemoryIDs* shmem_ids = get_shared_memory_ids(ids);
    shmem = attach_consumer_shared_memory(shmem_ids);

    int* buffer_flag = shmem->buffer_flag;
    //char* buffer = shmem-> buffer;
    int* turn = shmem->turn;
    int* flag = shmem->flag; /* Flag corresponding to each process in shared memory */

    //printf("flag[%d] %i\n", flag[i], i);

    do {


        do {
            flag[i] = want_in; // Raise my flag

            //printf("turn = %d\n", *turn);

            j = *turn; // Set local variable

            //printf("turn = %d\n", *turn);

            // wait until its my turn
            while ( j != i ) {
                j = (flag[j] != idle) ? *turn : (j + 1) % NUM_PROC;
            }

            // Declare intention to enter critical section
            flag[i] = in_cs;

            // Check that no one else is in critical section
            //fprintf(log_fp, "%s Check\n", get_timestamp());
            printf("Consumer: %s Checked\n", get_timestamp());
            for (j = 0; j < NUM_PROC; j++)
                if ((j != i) && (flag[j] == in_cs))
                    break;
        } while ((j < NUM_PROC) || (*turn != i && flag[*turn] != idle));

        // Assign turn to self and enter critical section
        *turn = i;

        for (i = 0; i < NUM_BUFFERS; i++) {
            if (buffer_flag[i] == 1) {
                // Buffer is full so read it
                buffer_idx = BUFFER_SIZE * i;
                printf("Consumer: %s Read \t %d \t Message\n", get_timestamp(), i);
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
        fprintf(log_fp, "%s Sleep \t %d\n", get_timestamp(), sleep_time);
        printf("Consumer: %s Sleep \t %d\n", get_timestamp(), sleep_time);
        sleep(sleep_time);

        } while (1);
}

void add_signal_handler() {
    struct sigaction act1;
    act1.sa_handler = sigint_handler;
    act1.sa_flags = 0;
    if ( ( sigemptyset(&act1.sa_mask) == -1) || (sigaction(SIGINT, &act1, NULL)  == -1) ) {
        perror("Failed to set up interrupt");
        exit(1);
    }
    struct sigaction act2;
    act2.sa_handler = eof_handler;
    act2.sa_flags = 0;
    if ( ( sigemptyset(&act2.sa_mask) == -1) || (sigaction(SIGUSR1, &act2, NULL)  == -1) ) {
        perror("Failed to set up interrupt");
        exit(1);
    }
}

void eof_handler(int s) {
//    fprintf(stderr, "Process # %d exiting because of signal # %d\n", getpid(), s);
//    fprintf(log_fp, "%s Terminated\tNormal\n", get_timestamp());
//    printf("%s Terminated\tNormal\n", get_timestamp());
//    if (log_fp != NULL) {
//        if (fclose(log_fp) != 0) {
//            perror("failed to close log file\n");
//            exit(1);
//        }
//
//    }

    detach_shared_memory(shmem);

    exit(1);
}

void sigint_handler(int s) {
//    fprintf(stderr, "Process # %d exiting because of signal # %d\n", getpid(), s);
//    fprintf(log_fp, "%s Terminated\tKilled\n", get_timestamp());
//    if (log_fp != NULL) {
//        if (fclose(log_fp) != 0) {
//            perror("failed to close log file\n");
//            exit(1);
//        }
//    }

    detach_shared_memory(shmem);

    exit(1);
}
