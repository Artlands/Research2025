#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <ucs/profile/profile_on.h>


#define OSHM_LOOP_SMALL_MR              500
#define OSHM_LOOP_LARGE_MR              50
#define LARGE_MESSAGE_SIZE              8192
#define MAX_MESSAGE_SIZE                (1 << 22)
#define MESSAGE_ALIGNMENT_MR            (1 << 12)

struct pe_vars {
    int me;
    int npes;
    int pairs;
    int nxtpe;
};

struct pe_vars init_openshmem(void)
{
    struct pe_vars v;

    shmem_init();
    v.me = shmem_my_pe();
    v.npes = shmem_n_pes();

    v.pairs = v.npes / 2;
    v.nxtpe = v.me < v.pairs ? v.me + v.pairs : v.me - v.pairs;

    return v;
}

double get_microseconds_ts() {
    double retval;
    struct timeval t;
    gettimeofday(&t, NULL);
    retval = (double) t.tv_sec * 1e6 + (double) t.tv_usec;
    return retval;
}

char *allocate_memory(int me, long align_size)
{
    char *msg_buffer;

    msg_buffer = (char *)shmem_malloc(MAX_MESSAGE_SIZE * OSHM_LOOP_LARGE_MR + align_size);

    if (NULL == msg_buffer) {
        fprintf(stderr, "Failed to shmalloc (pe: %d)\n", me);
        shmem_finalize();
        exit(1);
    }

    return msg_buffer;
}

char *align_memory(unsigned long address, int const align_size)
{
    return (char *)((address + (align_size - 1)) / align_size * align_size);
}

double message_rate(struct pe_vars v, char *buffer, size_t size, int iterations)
{
    double start, end;
    int i, offset;
    double mr = 0;

    memset(buffer, size, MAX_MESSAGE_SIZE * OSHM_LOOP_LARGE_MR);

    shmem_barrier_all();

    if (v.me < v.pairs) {
        start = get_microseconds_ts();
        
        for (i = 0, offset = 0; i < iterations; i++, offset += size) {
            shmem_getmem(&buffer[offset], &buffer[offset], size, v.nxtpe);
        }

        end = get_microseconds_ts();

        mr = 1e6 * ( ((double)size * iterations)/(1024 * 1024) ) / ((double)end - (double)start);

        printf("PE %d: Message rate for %ld-byte messages: %.6f MB/s\n", v.me, size, mr);
    }

    return 0;
}

int main(int argc, char **argv) {
    struct pe_vars v;
    char *msg_buffer, *aligned_buffer;
    long alignment;

    v = init_openshmem();

    if (v.npes < 2) {
        if (v.me == 0) {
            fprintf(stderr, "This benchmark requires two PEs.\n");
        }
        shmem_finalize();
        return 1;
    }

    if (argc != 2) {
        if (v.me == 0) {
            fprintf(stderr, "Usage: %s <message_size_in_bytes>\n", argv[0]);
        }
        shmem_finalize();
        return 1;
    }

    size_t msg_size = atol(argv[1]); // Message size in bytes
    int iterations = msg_size < LARGE_MESSAGE_SIZE ? OSHM_LOOP_SMALL_MR : OSHM_LOOP_LARGE_MR;

    // Allocate symmetric memory
    alignment = MESSAGE_ALIGNMENT_MR;
    msg_buffer = allocate_memory(v.me, alignment);
    aligned_buffer = align_memory((unsigned long)msg_buffer, alignment);
    memset(aligned_buffer, 0, MAX_MESSAGE_SIZE * OSHM_LOOP_LARGE_MR);

    message_rate(v, aligned_buffer, msg_size, iterations);

    shmem_free(msg_buffer);
    shmem_finalize();
    return 0;
}