#include <stdio.h>

#include "global_constants.h"
#include "shared_memory.h"
#include "helpers.h"


int main (int argc, char *argv[]) {
//    int i, j = 0;
//    i = atoi(argv[1]);
//    const int NUM_PROC = PROC_LIMIT;
//    add_signal_handler();
//
//    char** ids = split_string(argv[ID_STR_IDX], ",");
//
//    struct SharedMemoryIDs* shmem_ids = get_shared_memory_ids(ids);
//    struct SharedMemory* shmem = attach_shared_memory(shmem_ids);
//
//    printf("hello from consumer\n");
//
//    int turn = *shmem->turn;
//    int* flag[NUM_PROC] = shmem->flag;/* Flag corresponding to each process in shared memory */
//
//    do {
//        do {
//            flag[i] = want_in; // Raise my flag
//
//            j = turn; // Set local variable
//
//            // wait until its my turn
//            while ( j != i )
//            j = (flag[j] != idle) ? turn : (j + 1) % NUM_PROC;
//
//            // Declare intention to enter critical section
//            flag[i] = in_cs;
//
//            // Check that no one else is in critical section
//            for (j = 0; j < NUM_PROC; j++)
//                if ((j != i) && (flag[j] == in_cs))
//                    break;
//        } while ((j < NUM_PROC) || (turn != i && flag[turn] != idle));
//
//        // Assign turn to self and enter critical section
//        turn = i;
//        critical_section();
//
//        // Exit section
//        j = (turn + 1) % NUM_PROC;
//        while (flag[j] == idle)
//            j = (j + 1) % NUM_PROC;
//
//        // Assign turn to next waiting process; change own flag to idle
//        turn = j; flag[i] = idle;
//
//        remainder_section();
//        } while (1);
//
//    detach_shared_memory(shmem);
//
//    return 0;
}



