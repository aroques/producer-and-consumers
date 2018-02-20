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

struct SharedMemory* shmem;
FILE* fp;

int main (int argc, char *argv[]) {
    int i, j, sleep_time = 0;
    srand(time(0));
    
    i = atoi(argv[1]);
    const int NUM_PROC = atoi(argv[2]);

    printf("%s\n", get_timestamp());
    
    char filename[50] = "./prod.log";
    fp = fopen(filename, "w");
    fprintf(fp, "pid \tlogical process number\n");
    fprintf(fp, "%d\t%d\n\n", getpid(), i);

    printf("i: %d n: %d\n", i, NUM_PROC);

    add_signal_handler();

	char** ids = split_string(argv[ID_STR_IDX], ",");

    struct SharedMemoryIDs* shmem_ids = get_shared_memory_ids(ids);
    shmem = attach_shared_memory(shmem_ids);

    printf("hello from producer\n");

    //int* buffer_flag = shmem->buffer_flag;
    //char* buffer = shmem-> buffer;

	int* turn = shmem->turn;
	int* flag = shmem->flag; /* Flag corresponding to each process in shared memory */


	do {
        do {
            flag[i] = want_in; // Raise my flag

            j = *turn; // Set local variable

            // wait until its my turn
            printf("p: waiting until my turn\n");
            while ( j != i ) {
                j = (flag[j] != idle) ? *turn : (j + 1) % NUM_PROC;
            }

            // Declare intention to enter critical section
            flag[i] = in_cs;

            // Check that no one else is in critical section
            printf("p: checking that no one else is in cs before going in\n");
            for (j = 0; j < NUM_PROC; j++)
                if ((j != i) && (flag[j] == in_cs))
                    break;
        } while ((j < NUM_PROC) || (*turn != i && flag[*turn] != idle));

        // Assign turn to self and enter critical section
        *turn = i;
        printf("p: entering critical section\n");
        //critical_section();

        // Exit section
        j = (*turn + 1) % NUM_PROC;
        while (flag[j] == idle) {
            j = (j + 1) % NUM_PROC;
        }

        // Assign turn to next waiting process; change own flag to idle
        *turn = j; flag[i] = idle;

        printf("p: exited critical section\n");

        // Remainder section
        sleep_time = (rand() % 5) + 1;
        printf("p: sleeping for %d seconds\n", sleep_time);
        sleep(sleep_time);

        } while (1);

    detach_shared_memory(shmem);

    return 0;
}

void add_signal_handler() {
    struct sigaction act;
    act.sa_handler = sigint_handler;
    act.sa_flags = 0;
    if ( ( sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGINT, &act, NULL)  == -1) ) {
        perror("Failed to set up interrupt");
        exit(1);
    }
}

void sigint_handler(int s) {
    // kill children processes and abort
    fprintf(stderr, "\nsig num received: %d\n", s);
    fprintf(stderr, "exiting...\n");

    fclose (fp);

    detach_shared_memory(shmem);

    perror("test");
    exit(1);
}