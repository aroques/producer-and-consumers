#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "global_constants.h"

int main (int argc, char *argv[]) {

//    int segment_id = atoi(argv[1]);
//
//    /* Attach the shared memory segment. */
//    char* shared_memory = attach_shared_memory(segment_id);
    //int i = atoi(argv[1]);

	printf("hello from consumer!\n");
    printf("argv[0] = %s\n", argv[0]);
    printf("argv[1] = %s\n", argv[1]);
    printf("argv[2] = %s\n", argv[2]);
    printf("argv[3] = %s\n", argv[3]);
	//printf("my number is = %s!\n", argv[1]);

    return 0;
}


