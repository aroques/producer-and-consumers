#include <stdio.h>

#include "global_constants.h"
#include "shared_memory.h"
#include "helpers.h"


int main (int argc, char *argv[]) {

    add_signal_handler();

    char** ids = split_string(argv[ID_STR_IDX], ",");

    struct SharedMemoryIDs* shmem_ids = get_shared_memory_ids(ids);

    struct SharedMemory* shmem = attach_consumer_shared_memory(shmem_ids);

    detach_shared_memory(shmem);

    printf("hello from consumer\n");

    //printf("my number is = %s!\n", argv[1]);

    return 0;
}



