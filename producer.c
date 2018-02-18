#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[]) {

	printf("hello world!");

	// Randomly wake up and check if a buffer is free

//	extern int flag[2]; /* Shared variable; one for each process */
//	extern int turn;    /* Shared variable */
//
//	void process (const int me) { /* me can be 0 or 1 */
//		int other = 1 - me;
//		do {
//			/* Entry section */
//			flag[me] = 1; /* true */
//			turn = other   /* cede the turn */
//			while ( flag[other]  && turn == other);
//
//			critical_section();
//
//			flag[me] = 0; /* false */
//
//			remainder_section();
//
//		} while (1);
//	}


    return EXIT_SUCCESS;
}
