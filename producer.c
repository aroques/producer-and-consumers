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
void handle_sigterm(int s);
void open_files();

struct SharedMemory* shmem;
FILE* log_fp;
FILE* read_fp;

int main (int argc, char *argv[]) {
    int i, j, k, sleep_time, buffer_idx = 0;
    srand(time(0) ^ getpid());

    pid_t pid = getpid();
    
    i = atoi(argv[1]);

    const int NUM_PROC = atoi(argv[2]);
    char** ids = split_string(argv[ID_STR_IDX], ",");

    char buff[100];
    
    open_files();
    
    sprintf(buff, "Producer   : %s Started\n", get_timestamp());
    print_and_write(buff, log_fp);

    add_signal_handler();
    
    struct SharedMemoryIDs* shmem_ids = get_shared_memory_ids(ids);
    shmem = attach_shared_memory(shmem_ids);

    // Unpack shared memory pointers
    int* buffer_flag = shmem->buffer_flag;
    char* buffer = shmem-> buffer;
	int* turn = shmem->turn;
	int* flag = shmem->flag; /* Flag corresponding to each process in shared memory */
    int* end_program = shmem->end_program;

	do {

        if (*end_program) {
            printf("PRODUCER EXITING PROGRAM\n");
            sprintf(buff, "Producer   : %s Exiting program\n", get_timestamp());
            print_and_write(buff, log_fp);

            handle_sigterm(-1);
        }
        do {

            if (*end_program) {
                printf("PRODUCER EXITING PROGRAM\n");
                sprintf(buff, "Producer   : %s Exiting program\n", get_timestamp());
                print_and_write(buff, log_fp);

                handle_sigterm(-1);
            }
            printf("PRODUCER i %d RAISING FLAG TURN %d\n", i, *turn);
            flag[i] = want_in; // Raise my flag
            j = *turn; // Set local variable

            // wait until its my turn
            printf("PRODUCER WAITING FOR MY TURN\n");
            while ( j != i ) {
                j = (flag[j] != idle) ? *turn : (j + 1) % NUM_PROC;
            }

            // Declare intention to enter critical section

            printf("PRODUCER DECLARING INTENTION TO ENTER CRITICAL SECTION\n");
            flag[i] = in_cs;

            // Check that no one else is in critical section
            sprintf(buff, "Producer   : %s Check\n", get_timestamp());
            print_and_write(buff, log_fp);
            
            for (j = 0; j < NUM_PROC; j++)
                if ((j != i) && (flag[j] == in_cs))
                    break;

        } while ((j < NUM_PROC) || (*turn != i && flag[*turn] != idle));

        // Assign turn to self and enter critical section
        *turn = i;
        printf("PRODUCER INSIDE CRITICAL SECTION\n");

        /*
         *  In critical section
         */
        for (k = 0; k < NUM_BUFFERS; k++) {
            if (buffer_flag[k] == 0) {
                
                // Buffer is empty so fill it
                buffer_idx = BUFFER_SIZE * k;
                
                if ( fgets(&buffer[buffer_idx], 100, read_fp) == NULL ) {
                    // End of file
                    sprintf(buff, "Producer   : Flipping end program flag\n");
                    print_and_write(buff, log_fp);
                    
                    *end_program = 1;
                    
                    break;
                }
                else {
                    // We write a message
                    buffer_flag[k] = 1;
                    sprintf(buff, "Producer   : %s Write \t %d \t Message\n", get_timestamp(), k);
                    print_and_write(buff, log_fp);
                }
            }
        }
        printf("PRODUCER EXITING CRITICAL SECTION\n");

        printf("PRODUCER: printing flags\n");
        for (k = 0; k < NUM_PROC; k++) {
            printf("flag[%d] = %d\n", k, flag[k]);
        }

        printf("PRODUCER: j = (*turn = %d) + 1 %% %d\n", *turn, NUM_PROC);
        // Exit critical section
        j = (*turn + 1) % NUM_PROC;
        printf("j = %d\n", j);
        while (flag[j] == idle) {
            j = (j + 1) % NUM_PROC;
            printf("j = %d\n", j);
        }
        printf("PRODUCER ASSIGNING TURN TO %d\n", j);

        // Assign turn to next waiting process; change own flag to idle
        *turn = j; flag[i] = idle;

        // Remainder section
        sleep_time = get_sleep_time();
        sprintf(buff, "Producer   : %s Sleep \t %d\n", get_timestamp(), sleep_time);
        print_and_write(buff, log_fp);
        sleep(sleep_time);

        } while (1);
}

void add_signal_handler() {
    struct sigaction act;
    act.sa_handler = handle_sigterm; // Signal handler
    sigemptyset(&act.sa_mask);       // No other signals shoud be blocked
    act.sa_flags = 0;                // 0 so do not modify behavior
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

void handle_sigterm(int sig) {
    printf("Producer   : Caught SIGINT\n");
    detach_shared_memory(shmem);
    fclose(log_fp);
    fclose(read_fp);
    _exit(0);
}

void open_files() {
    log_fp = fopen("./prod.log", "w");
    if (log_fp == NULL) {
        perror("failed to open prod.log for writing\n");
        exit(1);
    }

    read_fp = fopen("./data.txt", "r");
    if (read_fp == NULL) {
        perror("failed to open data.txt for reading\n");
        exit(1);
    }
}
