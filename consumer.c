#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "functions.h"

int main (int argc, char *argv[]) {

//    int segment_id = atoi(argv[1]);
//
//    /* Attach the shared memory segment. */
//    char* shared_memory = attach_shared_memory(segment_id);

	printf("hello from consumer!\n");
	printf("my number is = %s!\n", argv[1]);

    return 0;
}


