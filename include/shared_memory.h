#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

struct SharedMemoryIDs {
    int buffer_id;
    int flag_id;
    int turn_id;
    int buffer_flag_id;
};

struct SharedMemory {
    char* buffer;
    int* flag;
    int* turn;
    int* buffer_flag;
};

struct SharedMemory* attach_shared_memory(struct SharedMemoryIDs* shmem_ids);
struct SharedMemory* attach_consumer_shared_memory(struct SharedMemoryIDs* shmem_ids);
struct SharedMemoryIDs* get_shared_memory();
void deallocate_shared_memory(struct SharedMemoryIDs* shmem_ids);
void detach_shared_memory(struct SharedMemory* shmem);
struct SharedMemoryIDs* get_shared_memory_ids(char** ids);
void cleanup_shared_memory(struct SharedMemoryIDs* shmem_ids, struct SharedMemory* shmem);
void initialize_shared_memory(struct SharedMemory* shmem);


#endif
