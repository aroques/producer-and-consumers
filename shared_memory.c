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
    ids->turn_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
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

struct SharedMemory* attach_consumer_shared_memory(struct SharedMemoryIDs* shmem_ids) {
    struct SharedMemory* shmem = malloc(sizeof(struct SharedMemory));

    shmem->buffer = (char*)shmat(shmem_ids->buffer_id, 0, SHM_RDONLY);
    shmem->flag = (int*)shmat(shmem_ids->flag_id, 0, 0);
    shmem->turn = (int*)shmat(shmem_ids->turn_id, 0, 0);
    shmem->buffer_flag = (int*)shmat(shmem_ids->buffer_flag_id, 0, 0);

    if ( !shmem->buffer | !shmem->flag | !shmem->turn | !shmem->buffer_flag ) {
        perror("shmat");
        exit(1);
    }

    return shmem;

}

struct SharedMemoryIDs* get_shared_memory_ids(char** ids) {
    struct SharedMemoryIDs* shmem_ids = malloc(sizeof(struct SharedMemoryIDs));
    shmem_ids->buffer_id = atoi(ids[0]);
    shmem_ids->flag_id = atoi(ids[1]);
    shmem_ids->turn_id = atoi(ids[2]);
    shmem_ids->buffer_flag_id = atoi(ids[3]);
    return shmem_ids;
}

void cleanup_shared_memory(struct SharedMemoryIDs* shmem_ids, struct SharedMemory* shmem) {
    deallocate_shared_memory(shmem_ids);
    detach_shared_memory(shmem);
}

void deallocate_shared_memory(struct SharedMemoryIDs* shmem_ids) {
    /* Deallocate shared memory segments */
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

void detach_shared_memory(struct SharedMemory* shmem) {
    /* Detach the shared memory segment. */
    if (shmdt(shmem->buffer) == -1 ) {
        perror("shmdt");
        exit(1);
    }
    if (shmdt(shmem->flag) == -1 ) {
        perror("shmdt");
        exit(1);
    }
    if (shmdt(shmem->turn) == -1 ) {
        perror("shmdt");
        exit(1);
    }
    if (shmdt(shmem->buffer_flag) == -1 ) {
        perror("shmdt");
        exit(1);
    }
}

void initialize_shared_memory(struct SharedMemory* shmem) {
    *shmem->turn = 0;
    for (int i = 0; i < PROC_LIMIT; i++) {
        shmem->flag[i] = 0;
    }
    for (int i = 0; i < NUM_BUFFERS; i++) {
        shmem->buffer_flag[i] = 0;
    }
}
