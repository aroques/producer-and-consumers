#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "global_constants.h"
#include "helpers.h"
#include "shared_memory.h"

int main (int argc, char *argv[]) {
    //int i, j, n = 0;
    add_signal_handler();

	char** ids = split_string(argv[2], ",");

    struct SharedMemoryIDs* shmem_ids = get_shared_memory_ids(ids);

    struct SharedMemory* shmem = attach_shared_memory(shmem_ids);

    cleanup_shared_memory(shmem_ids, shmem);

//
//	i = atoi(argv[1]);
//
//	enum state { idle, want_in, in_cs };
//	extern int turn;
//	extern state flag[n];/*Flag corresponding to each process in shared memory */
//    do {
//        do {
//            flag[i] = want_in; // Raise my flag
//
//            j = turn; // Set local variable
//
//            // wait until its my turn
//            while ( j != i )
//            j = (flag[j] != idle) ? turn : (j + 1) % n;
//
//            // Declare intention to enter critical section
//            flag[i] = in_cs;
//
//            // Check that no one else is in critical section
//            for (j = 0; j < n; j++)
//                if ((j != i) && (flag[j] == in_cs))
//                    break;
//        } while ((j < n) || (turn != i && flag[turn] != idle));
//
//        // Assign turn to self and enter critical section
//        turn = i;
//        critical_section();
//
//        // Exit section
//        j = (turn + 1) % n;
//        while (flag[j] == idle)
//            j = (j + 1) % n;
//
//        // Assign turn to next waiting process; change own flag to idle
//        turn = j; flag[i] = idle;
//
//        remainder_section();
//        } while (1);

//
    return 0;
}




