#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>

#include "global_constants.h"
#include "shared_memory.h"

struct SharedMemoryIDs* get_shared_memory() {
    struct SharedMemoryIDs* ids = malloc(sizeof(struct SharedMemoryIDs));

    ids->buffer_id = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | S_IRUSR | S_IWUSR); // children inherit shmem segments
    ids->flag_id = shmget(IPC_PRIVATE, sizeof(int)*PROC_LIMIT, IPC_CREAT | S_IRUSR | S_IWUSR);
    ids->turn_id = shmget(IPC_PRIVATE, sizeof(int)*2, IPC_CREAT | S_IRUSR | S_IWUSR);
    ids->buffer_flag_id = shmget(IPC_PRIVATE, sizeof(int)*5, IPC_CREAT | S_IRUSR | S_IWUSR);

    if ( (ids->buffer_id == -1) | (ids->flag_id == -1) |
            (ids->turn_id == -1) | (ids->buffer_flag_id == -1) ) {
        perror("shmget");
        exit(1);
    }

    return ids;
}

struct SharedMemory* attach_shared_memory(struct SharedMemoryIDs* shmem_ids) {
    struct SharedMemory* shmem = malloc(sizeof(struct SharedMemory));

    shmem->buffer = (char*)shmat(shmem_ids->buffer_id, 0, 0);
    shmem->flag = (int*)shmat(shmem_ids->flag_id, 0, 0);
    shmem->turn = (int*)shmat(shmem_ids->turn_id, 0, 0);
    shmem->buffer_flag = (int*)shmat(shmem_ids->buffer_flag_id, 0, 0);

    if ( !shmem->buffer | !shmem->flag | !shmem->turn | !shmem->buffer_flag ) {
        perror("shmat");
        exit(1);
    }

    return shmem;

}

void deallocate_shmem(struct SharedMemoryIDs* shmem_ids) {
    /* Deallocate the shared memory segment. */
    if (shmctl(shmem_ids->buffer_id, IPC_RMID, 0) == 1) {
        perror("shmctl");
        exit(1);
    }
    if (shmctl(shmem_ids->flag_id, IPC_RMID, 0) == 1) {
        perror("shmctl");
        exit(1);
    }
    if (shmctl(shmem_ids->turn_id, IPC_RMID, 0) == 1) {
        perror("shmctl");
        exit(1);
    }
    if (shmctl(shmem_ids->buffer_flag_id, IPC_RMID, 0) == 1) {
        perror("shmctl");
        exit(1);
    }
}

void cleanup_shmem(char* shared_memory, int* segment_id) {
    /* Detach the shared memory segment. */
    if (shmdt(shared_memory) == -1 ) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(*segment_id, IPC_RMID, 0) == 1) {
        perror("shmctl");
        exit(1);
    }
}

