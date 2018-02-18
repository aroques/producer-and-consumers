#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "global_constants.h"

int main (int argc, char *argv[]) {
    //int i, j, n = 0;
	printf("hello from producer!\n");
	printf("argv[0] = %s\n", argv[0]);
	printf("argv[1] = %s\n", argv[1]);
	printf("argv[2] = %s\n", argv[2]);
	printf("argv[3] = %s\n", argv[3]);
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
